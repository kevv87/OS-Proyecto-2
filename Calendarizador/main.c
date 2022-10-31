#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include "types.h"
#include "constants.h"
#include "calendarizador.c"
#include "Thread_Doubly_Linked_List.c"
#include "time.h"
#include <json-c/json.h>


///Globals
int initial_id=0;

int calendarization_algorithm=0;
int canal_algorithm=0;
int boat_quantity;
int canal_length;
int signboard_time=1;
int priority[2];
int edf_deadlines[3];


Canal_node_t* canal;
int base_speed=0;

int boats_remaining_left=0;
sem_t sem_boats_remaining_left;

int boats_remaining_right=0;
sem_t sem_boats_remaining_right;

int loop=1;

int boats_in_canal=0;
sem_t sem_boats_in_canal;

Boat_Doubly_Linked_List_t* list_creation_order;
sem_t sem_list_creation_order;//To debug general creation order

Boat_Doubly_Linked_List_t* list_exec_order_general;
sem_t sem_list_exec_order_general;//To debug total execution order

Boat_Doubly_Linked_List_t* list_exec_order_left;
sem_t sem_list_exec_order_left;//To force by busy waiting the left order

Boat_Doubly_Linked_List_t* list_exec_order_right;
sem_t sem_list_exec_order_right;//To force by busy waiting the left order

Boat_Doubly_Linked_List_t* list_left;
sem_t sem_list_left;

Boat_Doubly_Linked_List_t* list_right;
sem_t sem_list_right;

Thread_Doubly_Linked_List_t* thread_list_left;
Thread_Doubly_Linked_List_t* thread_list_right;

sem_t sem_left;//To block left boats
sem_t sem_right;//To block right boats
sem_t sem_direction_aux;//To switch direction

sem_t sem_schedule_left;
sem_t sem_schedule_right;

///RR
Boat_Doubly_Linked_List_t* list_RR_left;
sem_t sem_list_RR_left;

Boat_Doubly_Linked_List_t* list_RR_right;
sem_t sem_list_RR_right;



///Defs
int get_id();
void print_semaphore(sem_t* sem);
void initial_load(Load_t* load);
void canal_init();
void print_canal();
void ups(sem_t* sem, int value);
void set_to_zero(sem_t* sem);
void* boat_spawner_func();
void* equity_thread_func(void* arguments_);
void* equity_thread_launcher_func(void* arguments_);


int get_id()
{
    return initial_id++;
}
void print_semaphore(sem_t* sem)
{
    int sem_value;
    sem_getvalue(&(*sem), &sem_value);
    printf("Sem value: %d\n", sem_value);
}
void initial_load(Load_t* load)
{
    while(load->left[0] > 0)
    {
        Boat_t* normal_boat = malloc(sizeof(Boat_t));
        normal_boat->id=get_id(); normal_boat->type=0; normal_boat->position=0; normal_boat->direction=RIGHT; normal_boat->speed=base_speed; normal_boat->deadline=edf_deadlines[0]; normal_boat->priority=priority[0]; //normal_boat->arrival_time=0;
        append_boat(list_left, normal_boat);
        load->left[0]--;
    }
    while(load->left[1] > 0)
    {
        Boat_t* fisher_boat = malloc(sizeof(Boat_t));
        fisher_boat->id=get_id(); fisher_boat->type=1; fisher_boat->position=0; fisher_boat->direction=RIGHT; fisher_boat->speed=base_speed*FISHER_BOAT_SPEED_MULTIPLIER;fisher_boat->deadline=edf_deadlines[1]; fisher_boat->priority=priority[1]; //fisher_boat->arrival_time=0;
        append_boat(list_left, fisher_boat);
        load->left[1]--;
    }
    while(load->left[2] > 0)
    {
        Boat_t* patrol_boat = malloc(sizeof(Boat_t));
        patrol_boat->id=get_id(); patrol_boat->type=2; patrol_boat->position=0; patrol_boat->direction=RIGHT; patrol_boat->speed=base_speed*PATROL_BOAT_SPEED_MULTIPLIER; patrol_boat->deadline=edf_deadlines[2]; patrol_boat->priority=PATROL_BOAT_PRIORITY;
        append_boat(list_left, patrol_boat);
        load->left[2]--;
    }

    while(load->right[0] > 0)
    {
        Boat_t* normal_boat = malloc(sizeof(Boat_t));
        normal_boat->id=get_id(); normal_boat->type=0; normal_boat->position=0; normal_boat->direction=LEFT; normal_boat->speed=base_speed; normal_boat->deadline=edf_deadlines[0]; normal_boat->priority=priority[0];
        append_boat(list_right, normal_boat);
        load->right[0]--;
    }
    while(load->right[1] > 0)
    {
        Boat_t* fisher_boat = malloc(sizeof(Boat_t));
        fisher_boat->id=get_id(); fisher_boat->type=1; fisher_boat->position=0; fisher_boat->direction=LEFT; fisher_boat->speed=base_speed*FISHER_BOAT_SPEED_MULTIPLIER;fisher_boat->deadline=edf_deadlines[1]; fisher_boat->priority=priority[1];
        append_boat(list_right, fisher_boat);
        load->right[1]--;
    }
    while(load->right[2] > 0)
    {
        Boat_t* patrol_boat = malloc(sizeof(Boat_t));
        patrol_boat->id=get_id(); patrol_boat->type=2; patrol_boat->position=0; patrol_boat->direction=LEFT; patrol_boat->speed=base_speed*PATROL_BOAT_SPEED_MULTIPLIER; patrol_boat->deadline=edf_deadlines[2]; patrol_boat->priority=PATROL_BOAT_PRIORITY;
        append_boat(list_right, patrol_boat);
        load->right[2]--;
    }
}
void canal_init()
{
    canal=malloc(canal_length * sizeof(Canal_node_t));
    for(int i=0; i<canal_length; i++)
    {
        sem_init(&((canal+i)->sem), 0, 1);
        canal[i].boat=NULL;
    }
}
void print_canal()
{
    int value;
    for(int i=0; i<canal_length; i++)
    {
        sem_getvalue(&((canal+i)->sem), &value);
        printf("Canal Sem: %d = %d\t", i, value);
        if((canal+i)->boat!=NULL)
        {
            print_boat((canal+i)->boat);
        }
        else
        {
            printf("\tBoat: NULL\n");
        }
    }
}
void ups(sem_t* sem, int value)
{
    int sem_value;
    sem_getvalue(&(*sem), &sem_value);
    while(sem_value<value)
    {
        sem_post(&(*sem));
        sem_getvalue(&(*sem), &sem_value);
    }
}
void set_to_zero(sem_t* sem)
{
    int sem_value;
    sem_getvalue(&(*sem), &sem_value);
    while(sem_value>0)
    {
        sem_wait(&(*sem));
        sem_getvalue(&(*sem), &sem_value);
    }
}
void* boat_spawner_func()
{
    char caracter;
    while(loop)
    {
        //printf("Waiting for user:\n");
        caracter=(char)getchar();
        //printf("caracter: %c\n", caracter);
        Boat_t* boat = malloc(sizeof(Boat_t));
        switch(caracter)
        {
            case 'a':///Patrol Left
                boat->id=get_id(); boat->type=2; boat->position=0; boat->direction=RIGHT; boat->speed=base_speed*PATROL_BOAT_SPEED_MULTIPLIER; boat->deadline=edf_deadlines[2]; boat->priority=PATROL_BOAT_PRIORITY;
                sem_wait(&sem_list_left); append_boat(list_left, boat); sem_post(&sem_list_left);
                printf("Patrol Boat [%d] spawns on left\n", boat->id);
                sem_wait(&sem_boats_remaining_left); boats_remaining_left++; sem_post(&sem_boats_remaining_left);
                break;
            case 's':///Fisher Left
                boat->id=get_id(); boat->type=1; boat->position=0; boat->direction=RIGHT; boat->speed=base_speed*FISHER_BOAT_SPEED_MULTIPLIER; boat->deadline=edf_deadlines[1]; boat->priority=priority[1];
                sem_wait(&sem_list_left); append_boat(list_left, boat); sem_post(&sem_list_left);
                printf("Fisher Boat [%d] spawns on left\n", boat->id);
                sem_wait(&sem_boats_remaining_left); boats_remaining_left++; sem_post(&sem_boats_remaining_left);
                break;
            case 'd':///Normal Left
                boat->id=get_id(); boat->type=0; boat->position=0; boat->direction=RIGHT; boat->speed=base_speed; boat->deadline=edf_deadlines[0]; boat->priority=priority[0];
                sem_wait(&sem_list_left); append_boat(list_left, boat); sem_post(&sem_list_left);
                printf("Normal Boat [%d] spawns on left\n", boat->id);
                sem_wait(&sem_boats_remaining_left); boats_remaining_left++; sem_post(&sem_boats_remaining_left);
                break;
            case 'j':///Normal Right
                boat->id=get_id(); boat->type=0; boat->position=0; boat->direction=LEFT; boat->speed=base_speed; boat->deadline=edf_deadlines[0]; boat->priority=priority[0];
                sem_wait(&sem_list_right); append_boat(list_right, boat); sem_post(&sem_list_right);
                printf("Normal Boat [%d] spawns on right\n", boat->id);
                sem_wait(&sem_boats_remaining_right); boats_remaining_right++; sem_post(&sem_boats_remaining_right);
                break;
            case 'k':///Fisher Right
                boat->id=get_id(); boat->type=1; boat->position=0; boat->direction=LEFT; boat->speed=base_speed*FISHER_BOAT_SPEED_MULTIPLIER; boat->deadline=edf_deadlines[1]; boat->priority=priority[1];
                sem_wait(&sem_list_right); append_boat(list_right, boat); sem_post(&sem_list_right);
                printf("Fisher Boat [%d] spawns on right\n", boat->id);
                sem_wait(&sem_boats_remaining_right); boats_remaining_right++; sem_post(&sem_boats_remaining_right);
                break;
            case 'l':///Patrol Right
                boat->id=get_id(); boat->type=2; boat->position=0; boat->direction=LEFT; boat->speed=base_speed*PATROL_BOAT_SPEED_MULTIPLIER; boat->deadline=edf_deadlines[2]; boat->priority=PATROL_BOAT_PRIORITY;
                sem_wait(&sem_list_right); append_boat(list_right, boat); sem_post(&sem_list_right);
                printf("Patrol Boat [%d] spawns on right\n", boat->id);
                sem_wait(&sem_boats_remaining_right); boats_remaining_right++; sem_post(&sem_boats_remaining_right);
                break;

            default:
                if(caracter=='w')
                {
                    loop=0;
                };
        }
    }
}
void* RR_func(void* arguments_)
{
    Tico_arguments_launcher_t* arguments=arguments_;
    while(loop)
    {
        usleep(QUANTUM_TIME);
        if(arguments->direction==RIGHT)
        {
            if(!is_boat_list_empty(list_RR_left))
            {
                sem_wait(&sem_list_RR_left);
                append_boat(list_RR_left, list_RR_left->first->data);
                delete_first(list_RR_left);
                sem_post(&sem_list_RR_left);

                printf("List RR left:\t");
                print_list(list_RR_left);
                printf("\n");
            }
        }
        else
        {
            if(!is_boat_list_empty(list_RR_right))
            {
                sem_wait(&sem_list_RR_right);
                append_boat(list_RR_right, list_RR_right->first->data);
                delete_first(list_RR_right);
                sem_post(&sem_list_RR_right);

                //printf("List RR right:\t");
                //print_list(list_RR_right);
                //printf("\n");
            }
        }
    }
}
void* equity_thread_func(void* arguments_)
{
    ///adquiere los parametros
    Equity_arguments_t* arguments = arguments_;

    ///Direction semaphores
    if(arguments->boat->direction==RIGHT)
    {
        sem_wait(&sem_direction_aux);///Se apropia de la edicion de los semaforos de direccion
        int sem_value; sem_getvalue(&sem_left, &sem_value);
        if(boats_in_canal==0 && sem_value==0 && boats_remaining_right==0)
        {
            //*(arguments->finished_boats_left)=0;
            ups(&sem_left, arguments->w);
            set_to_zero(&sem_right);
        }
        sem_post(&sem_direction_aux);///Termina de apropiarce de la edicion de los semaforos de direccion

        ///Busy waiting hasta que le toque su turno
        int order=1;
        while(order)
        {
            sem_wait(&sem_list_exec_order_left);
            if(list_exec_order_left->first->data->id==arguments->boat->id)
            {
                order=0;
            }
            sem_post(&sem_list_exec_order_left);
        }order=1;
        int w_aux=1;
        while(w_aux)
        {
            sem_wait(&(*arguments->sem_finished_boats));
            if(*(arguments->finished_boats)<arguments->w)
            {
                w_aux=0;
            }
            sem_post(&(*arguments->sem_finished_boats));
        }w_aux=1;
        sem_wait(&sem_left);///Se come un permiso de entrar por la izquierda

        sem_wait(&(*arguments->sem_finished_boats)); *(arguments->finished_boats)=*(arguments->finished_boats)+1; sem_post(&(*arguments->sem_finished_boats));

    }
    else
    {
        sem_wait(&sem_direction_aux);///Se apropia de la edicion de los semaforos de direccion
        int sem_value; sem_getvalue(&sem_right, &sem_value);
        if(boats_in_canal==0 && sem_value==0 && boats_remaining_left==0)
        {
            //*(arguments->finished_boats_right)=0;
            ups(&sem_right, arguments->w);
            set_to_zero(&sem_left);
        }
        sem_post(&sem_direction_aux);///Se apropia de la edicion de los semaforos de direccion

        ///Busy waiting hasta que le toque su turno
        int order=1;
        while(order)
        {
            sem_wait(&sem_list_exec_order_right);
            if(list_exec_order_right->first->data->id==arguments->boat->id)
            {
                order=0;
            }
            sem_post(&sem_list_exec_order_right);
        }order=1;

        int w_aux=1;
        while(w_aux)
        {
            sem_wait(&(*arguments->sem_finished_boats));
            if(*(arguments->finished_boats)<arguments->w)
            {
                w_aux=0;
            }
            sem_post(&(*arguments->sem_finished_boats));
        }w_aux=1;

        sem_wait(&sem_right);///Se come un permiso de entrar por la derecha

        sem_wait(&(*arguments->sem_finished_boats)); *(arguments->finished_boats)=*(arguments->finished_boats)+1; sem_post(&(*arguments->sem_finished_boats));

    }

    int RR_aux=1;

    ///Recorre el canal
    for(int i=0; i<=canal_length; i++)
    {
        if(arguments->boat->direction==RIGHT)///Si el barco va para la derecha
        {
            if(calendarization_algorithm==0)
            {
                if(!is_boat_list_empty(list_RR_left))
                {
                    while(RR_aux)
                    {
                        sem_wait(&sem_list_RR_left);
                        if(list_RR_left->first->data->id==arguments->boat->id)
                        {
                            RR_aux=0;
                        }
                        sem_post(&sem_list_RR_left);
                    }
                    printf("%d moves in quantum\n", arguments->boat->id);
                    RR_aux=1;
                }
            }

            arguments->boat->position=i;///Actualiza la posicion del barco
            if(i==canal_length)///Final
            {
                if((canal+i-1)->boat->id == arguments->boat->id)///Si el barco de la posicion anterior es si mismo
                {
                    (canal+i-1)->boat=NULL;///Se borra
                }
                printf("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tSale: %d\n", arguments->boat->id);//todo/////////////////////////////////////////////////////////////////////////////
                sem_post(&((canal+i-1)->sem));///Libera el espacio anterior
            }
            else if(i==0)///Inicio
            {
                sem_wait(&((canal+i)->sem));///Reserva el espacio siguiente
                printf("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tEntra: %d\n", arguments->boat->id);

                ///Deletes first exec order left
                //sem_wait(&sem_list_exec_order_left); delete_first(list_exec_order_left); sem_post(&sem_list_exec_order_left);
                sem_wait(&sem_list_exec_order_left); delete_boat_for_id(list_exec_order_left, arguments->boat->id); sem_post(&sem_list_exec_order_left);

                ///Append general exec order
                sem_wait(&sem_list_exec_order_general); append_boat(list_exec_order_general, arguments->boat); sem_post(&sem_list_exec_order_general);
                sem_wait(&sem_boats_in_canal); boats_in_canal++; sem_post(&sem_boats_in_canal);

                (canal+i)->boat=arguments->boat;///Pone el bote en esa posicion
            }
            else///En el medio
            {
                sem_wait(&((canal+i)->sem));///Reserva el espacio siguiente
                if((canal+i-1)->boat->id == arguments->boat->id)///Si el barco de la posicion anterior es si mismo
                {
                    (canal+i-1)->boat=NULL;///Se borra
                }
                (canal+i)->boat=arguments->boat;///Pone el bote en la posicion
                sem_post(&((canal+i-1)->sem));///Libera el espacio anterior
            }
        }
        else///Si el barco va para la izquierda
        {
            if(calendarization_algorithm==0)
            {
                if(!is_boat_list_empty(list_RR_right))
                {
                    while(RR_aux)
                    {
                        sem_wait(&sem_list_RR_right);
                        if(list_RR_right->first->data->id==arguments->boat->id)
                        {
                            RR_aux=0;
                        }
                        sem_post(&sem_list_RR_right);
                    }
                    printf("%d moves in quantum\n", arguments->boat->id);
                    RR_aux=1;
                }
            }

            int i_upside_down=(canal_length-i-1);///i de derecha a izquierda (i al reves)
            arguments->boat->position=i_upside_down;///Actualiza la posicion del barco

            if(i==canal_length)///Final
            {
                if((canal+i_upside_down+1)->boat->id == arguments->boat->id)///Si el barco de la posicion anterior es si mismo
                {
                    (canal+i_upside_down+1)->boat=NULL;///Se borra
                }
                printf("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tSale: %d\n", arguments->boat->id);//todo/////////////////////////////////////////////////////////////////////////////
                sem_post(&((canal+i_upside_down+1)->sem));///Libera el espacio anterior
            }
            else if(i==0)///Inicio
            {
                sem_wait(&((canal+i_upside_down)->sem));///Reserva el espacio siguiente
                printf("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tEntra: %d\n", arguments->boat->id);

                ///Deletes first exec order right
                //sem_wait(&sem_list_exec_order_right); delete_first(list_exec_order_right); sem_post(&sem_list_exec_order_right);
                sem_wait(&sem_list_exec_order_right); delete_boat_for_id(list_exec_order_right, arguments->boat->id); sem_post(&sem_list_exec_order_right);

                ///Append general exec order
                sem_wait(&sem_list_exec_order_general); append_boat(list_exec_order_general, arguments->boat); sem_post(&sem_list_exec_order_general);
                sem_wait(&sem_boats_in_canal); boats_in_canal++; sem_post(&sem_boats_in_canal);

                (canal+i_upside_down)->boat=arguments->boat;///Pone el bote en esa posicion
            }
            else///En el medio
            {
                sem_wait(&((canal+i_upside_down)->sem));///Reserva el espacio siguiente
                if((canal+i_upside_down+1)->boat->id == arguments->boat->id)///Si el barco de la posicion anterior es si mismo
                {
                    (canal+i_upside_down+1)->boat=NULL;///Se borra
                }
                (canal+i_upside_down)->boat=arguments->boat;///Pone el bote en la posicion
                sem_post(&((canal+i_upside_down+1)->sem));///Libera el espacio anterior
            }
        }
        usleep((int)((1000000 / arguments->boat->speed) * TIME_FIX_FACTOR));///Simula el tiempo que le toma moverse un espacio
    }

    sem_wait(&sem_boats_in_canal); boats_in_canal--; sem_post(&sem_boats_in_canal);

    if(calendarization_algorithm==0)
    {
        if(arguments->boat->direction==RIGHT)
        {
            sem_wait(&sem_list_RR_left); delete_boat_for_id(list_RR_left, arguments->boat->id); sem_post(&sem_list_RR_left);
        }
        else
        {
            sem_wait(&sem_list_RR_right); delete_boat_for_id(list_RR_right, arguments->boat->id); sem_post(&sem_list_RR_right);
        }
    }

    sem_wait(&sem_direction_aux);///Se apropia de la edicion de los semaforos de direccion
    if(arguments->boat->direction==RIGHT)///Si el barco va para derecha
    {
        ///Baja el numero total contemporaneo de barcos que quedan por pasar desde la izquierda
        sem_wait(&sem_boats_remaining_left); boats_remaining_left--; sem_post(&sem_boats_remaining_left);
    }
    else///Si el barco va para izquierda
    {
        ///Baja el numero total contemporaneo de barcos que quedan por pasar desde la derecha
        sem_wait(&sem_boats_remaining_right); boats_remaining_right--; sem_post(&sem_boats_remaining_right);
    }

    if(arguments->boat->direction==RIGHT)///Si el barco va para la derecha
    {
        printf("A\n");
        if(*(arguments->finished_boats)==1 || boats_remaining_left==0)///Si ya pasaron los w barcos o no hay mas barcos que pasar desde la izquierda
        {
            printf("B\n");
            if(*(arguments->finished_boats)==arguments->w){printf("B1\n");}
            if(boats_remaining_left==0){printf("B2\n");}
            if(boats_remaining_right==0)///Si no hay mas barcos que pasar desde la derecha
            {
                printf("C\n");
                ups(&sem_left, arguments->w);///Se reestablece permisos a si mismo
                set_to_zero(&sem_right);
            }
            else///Quedan barcos por pasar del otro lado
            {
                printf("D\n");
                ups(&sem_right, arguments->w);///Reestablece el semaforo de la derecha pq hay que cambiar de sentido
                set_to_zero(&sem_left);
            }
            sem_wait(&(*arguments->sem_finished_boats)); *(arguments->finished_boats)=*(arguments->finished_boats)=0; sem_post(&(*arguments->sem_finished_boats));
        }
        else///Si no han pasado w barcos y hay barcos a la derecha que quieren pasar
        {
            printf("E\n");
            sem_wait(&(*arguments->sem_finished_boats)); *(arguments->finished_boats)=*(arguments->finished_boats)-1; sem_post(&(*arguments->sem_finished_boats));
        }
    }
    else///Si el barco va para la izquierda
    {
        printf("F\n");
        if(*(arguments->finished_boats)==1 || boats_remaining_right==0)///Ya pasaron w-1 barcos o no quedan barcos por pasar
        {
            printf("G\n");
            if(boats_remaining_left==0)///No quedan barcos al otro lado
            {
                printf("H\n");
                ups(&sem_right, arguments->w);
                set_to_zero(&sem_left);
            }
            else///Quedan barcos al otro lado
            {
                printf("I\n");
                ups(&sem_left, arguments->w);
                set_to_zero(&sem_right);
            }
            sem_wait(&(*arguments->sem_finished_boats)); *(arguments->finished_boats)=*(arguments->finished_boats)=0; sem_post(&(*arguments->sem_finished_boats));
        }
        else///No han pasado w-1 barcos y aun quedan barcos de este lado por pasar
        {
            printf("J\n");
            sem_wait(&(*arguments->sem_finished_boats)); *(arguments->finished_boats)=*(arguments->finished_boats)-1; sem_post(&(*arguments->sem_finished_boats));
            ///*(finished_boats)++; //todo WHY NOT?
        }
    }
    sem_post(&sem_direction_aux);
    printf("J1 -> finished_boats: %d\n", *(arguments->finished_boats));

}
void* equity_thread_launcher_func(void* arguments_)
{
    Equity_arguments_launcher_t* arguments=arguments_;
    Boat_Doubly_Linked_List_t* list;
    if(arguments->direction==RIGHT)
    {
        list=list_left;
    }
    else
    {
        list=list_right;
    }
    while(loop)//todo hacer q otro thread cambie una variable booleana global <continue> que cambie con un getchar()
    //while(1)
    {
        if(!is_boat_list_empty(list))
        {
            ///Creating boat thread
            pthread_t thread_boat;
            Equity_arguments_t* equity_arguments = malloc(sizeof(Equity_arguments_t));
            equity_arguments->w=arguments->w;
            equity_arguments->finished_boats=arguments->finished_boats;
            equity_arguments->sem_finished_boats=arguments->sem_finished_boats;
            equity_arguments->boat=list->first->data;

            ///Append creation order list
            sem_wait(&sem_list_creation_order); append_boat(list_creation_order, equity_arguments->boat); sem_post(&sem_list_creation_order);

            ///Exec order & calendarization
            if(equity_arguments->boat->direction==RIGHT)
            {
                sem_wait(&sem_list_exec_order_left); append_boat(list_exec_order_left, equity_arguments->boat); sem_post(&sem_list_exec_order_left);

                ///RR Left
                if(calendarization_algorithm==0)
                {
                    sem_wait(&sem_list_RR_left); append_boat(list_RR_left, equity_arguments->boat); sem_post(&sem_list_RR_left);
                }
                else
                {
                    sem_wait(&sem_schedule_left); schedule(calendarization_algorithm,list_exec_order_left); sem_post(&sem_schedule_left);
                }
            }
            else
            {
                sem_wait(&sem_list_exec_order_right); append_boat(list_exec_order_right, equity_arguments->boat); sem_post(&sem_list_exec_order_right);

                ///RR Right
                if(calendarization_algorithm==0)
                {
                    sem_wait(&sem_list_RR_right); append_boat(list_RR_right, equity_arguments->boat); sem_post(&sem_list_RR_right);
                }
                else
                {
                    sem_wait(&sem_schedule_right); schedule(calendarization_algorithm,list_exec_order_right); sem_post(&sem_schedule_right);
                }
            }

            ///Create thread
            pthread_create(&thread_boat, NULL, equity_thread_func, equity_arguments);
            printf("Created thread for Boat: %d\n", equity_arguments->boat->id);//todo/////////////////////////////////////////////////////////////////////////////

            ///Append thread list
            if(arguments->direction==RIGHT)
            {
                append_thread(thread_list_left, thread_boat);
                sem_wait(&sem_list_left); delete_first(list); sem_post(&sem_list_left);
            }
            else
            {
                append_thread(thread_list_right, thread_boat);
                sem_wait(&sem_list_right); delete_first(list); sem_post(&sem_list_right);
            }
        }
    }
}
void equity(int w)
{
    ///Print Equity value
    printf("Equity\n");
    printf("W: %d\n", w);


    ///Int for boats count to switch direction semaphores
    int* finished_boats = malloc(sizeof(int)); *finished_boats=0;
    sem_t sem_finished_boats; sem_init(&sem_finished_boats, 0, 1);

    ///Semaphores
    if(is_boat_list_empty(list_left))
    {
        sem_init(&sem_left, 0, 0);//Left boats starts bu default
        sem_init(&sem_right, 0, w);
    }
    else
    {
        sem_init(&sem_left, 0, w);//Left boats starts bu default
        sem_init(&sem_right, 0, 0);
    }


    ///Print oceans
    printf("Left:\t"); print_list(list_left);
    printf("Right:\t"); print_list(list_right);


    ///Print semaphores
    printf("Sem Left: "); print_semaphore(&sem_left);
    printf("Sem right: "); print_semaphore(&sem_right);


    boats_remaining_left=get_length(list_left);
    boats_remaining_right=get_length(list_right);


    ///Creating Launcher thread for left boats
    pthread_t thread_launcher_left;
    Equity_arguments_launcher_t* thread_launcher_arguments_left = malloc(sizeof(Equity_arguments_launcher_t));
    thread_launcher_arguments_left->direction=RIGHT;
    thread_launcher_arguments_left->w=w;
    thread_launcher_arguments_left->finished_boats=finished_boats;
    thread_launcher_arguments_left->sem_finished_boats=&sem_finished_boats;
    pthread_create(&thread_launcher_left, NULL, equity_thread_launcher_func, thread_launcher_arguments_left);


    ///Creating Launcher thread for right boats
    pthread_t thread_launcher_right;
    Equity_arguments_launcher_t* thread_launcher_arguments_right = malloc(sizeof(Equity_arguments_launcher_t));
    thread_launcher_arguments_right->direction=LEFT;
    thread_launcher_arguments_right->w=w;
    thread_launcher_arguments_right->finished_boats=finished_boats;
    thread_launcher_arguments_right->sem_finished_boats=&sem_finished_boats;
    pthread_create(&thread_launcher_right, NULL, equity_thread_launcher_func, thread_launcher_arguments_right);


    ///Creating threads for RR control
    if(calendarization_algorithm==0)
    {
        pthread_t RR_thread_left;
        Tico_arguments_launcher_t* RR_arguments_left=malloc(sizeof(Tico_arguments_launcher_t));
        RR_arguments_left->direction=RIGHT;
        pthread_create(&RR_thread_left, NULL, RR_func, RR_arguments_left);

        pthread_t RR_thread_right;
        Tico_arguments_launcher_t* RR_arguments_right=malloc(sizeof(Tico_arguments_launcher_t));
        RR_arguments_right->direction=LEFT;
        pthread_create(&RR_thread_right, NULL, RR_func, RR_arguments_right);
    }


    ///Waiting for launcher threads
    pthread_join(thread_launcher_left, NULL);
    pthread_join(thread_launcher_right, NULL);
}
void* sign_thread_func(void* arguments_)
{
    ///adquiere los parametros
    Sign_arguments_t* arguments = arguments_;

    ///Direction semaphores
    if(arguments->boat->direction==RIGHT)
    {
        sem_wait(&sem_direction_aux);///Se apropia de la edicion de los semaforos de direccion
        int sem_value; sem_getvalue(&sem_left, &sem_value);
        if(boats_in_canal==0 && sem_value==0 && boats_remaining_right==0)
        {
            ups(&sem_left, canal_length);
            set_to_zero(&sem_right);
        }
        sem_post(&sem_direction_aux);///Se apropia de la edicion de los semaforos de direccion

        ///Busy waiting hasta que le toque su turno
        while((list_exec_order_left->first->data->id!=arguments->boat->id) || (*(arguments->change_sign_flag)==1 && boats_remaining_right!=0))
        {}
        sem_wait(&sem_left);///Se come un permiso de entrar por la izquierda
    }
    else
    {
        sem_wait(&sem_direction_aux);///Se apropia de la edicion de los semaforos de direccion
        int sem_value; sem_getvalue(&sem_right, &sem_value);
        if(boats_in_canal==0 && sem_value==0 && boats_remaining_left==0)
        {
            ups(&sem_right, canal_length);
            set_to_zero(&sem_left);
        }
        sem_post(&sem_direction_aux);///Se apropia de la edicion de los semaforos de direccion

        ///Busy waiting hasta que le toque su turno
        while((list_exec_order_right->first->data->id!=arguments->boat->id) || (*(arguments->change_sign_flag)==1 && boats_remaining_left!=0))
        {}
        sem_wait(&sem_right);///Se come un permiso de entrar por la derecha
    }

    int RR_aux=1;

    ///Recorre el canal
    for(int i=0; i<=canal_length; i++)
    {
        if(arguments->boat->direction==RIGHT)///Si el barco va para la derecha
        {
            if(calendarization_algorithm==0)
            {
                if(!is_boat_list_empty(list_RR_left))
                {
                    while(RR_aux)
                    {
                        sem_wait(&sem_list_RR_left);
                        if(list_RR_left->first->data->id==arguments->boat->id)
                        {
                            RR_aux=0;
                        }
                        sem_post(&sem_list_RR_left);
                    }
                    printf("%d moves in quantum\n", arguments->boat->id);
                    RR_aux=1;
                }
            }

            arguments->boat->position=i;///Actualiza la posicion del barco
            if(i==canal_length)///Final
            {
                if((canal+i-1)->boat->id == arguments->boat->id)///Si el barco de la posicion anterior es si mismo
                {
                    (canal+i-1)->boat=NULL;///Se borra
                }
                printf("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tSale: %d\n", arguments->boat->id);//todo/////////////////////////////////////////////////////////////////////////////
                sem_post(&((canal+i-1)->sem));///Libera el espacio anterior
            }
            else if(i==0)///Inicio
            {
                sem_wait(&((canal+i)->sem));///Reserva el espacio siguiente

                printf("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tEntra: %d\n", arguments->boat->id);

                sem_wait(&sem_list_exec_order_left);
                    delete_first(list_exec_order_left);
                sem_post(&sem_list_exec_order_left);

                sem_wait(&sem_list_exec_order_general); append_boat(list_exec_order_general, arguments->boat); sem_post(&sem_list_exec_order_general);
                sem_wait(&sem_boats_in_canal); boats_in_canal++; sem_post(&sem_boats_in_canal);

                (canal+i)->boat=arguments->boat;///Pone el bote en esa posicion
            }
            else///En el medio
            {
                sem_wait(&((canal+i)->sem));///Reserva el espacio siguiente
                if((canal+i-1)->boat->id == arguments->boat->id)///Si el barco de la posicion anterior es si mismo
                {
                    (canal+i-1)->boat=NULL;///Se borra
                }
                (canal+i)->boat=arguments->boat;///Pone el bote en la posicion
                sem_post(&((canal+i-1)->sem));///Libera el espacio anterior
            }
        }
        else///Si el barco va para la izquierda
        {
            if(calendarization_algorithm==0)
            {
                if(!is_boat_list_empty(list_RR_right))
                {
                    while(RR_aux)
                    {
                        sem_wait(&sem_list_RR_right);
                        if(list_RR_right->first->data->id==arguments->boat->id)
                        {
                            RR_aux=0;
                        }
                        sem_post(&sem_list_RR_right);
                    }
                    printf("%d moves in quantum\n", arguments->boat->id);
                    RR_aux=1;
                }
            }

            int i_upside_down=(canal_length-i-1);///i de derecha a izquierda (i al reves)
            arguments->boat->position=i_upside_down;///Actualiza la posicion del barco

            if(i==canal_length)///Final
            {
                if((canal+i_upside_down+1)->boat->id == arguments->boat->id)///Si el barco de la posicion anterior es si mismo
                {
                    (canal+i_upside_down+1)->boat=NULL;///Se borra
                }
                printf("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tSale: %d\n", arguments->boat->id);//todo/////////////////////////////////////////////////////////////////////////////
                sem_post(&((canal+i_upside_down+1)->sem));///Libera el espacio anterior
            }
            else if(i==0)///Inicio
            {
                sem_wait(&((canal+i_upside_down)->sem));///Reserva el espacio siguiente

                printf("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tEntra: %d\n", arguments->boat->id);

                sem_wait(&sem_list_exec_order_right);
                delete_first(list_exec_order_right);
                sem_post(&sem_list_exec_order_right);
                //printf("%d despues de sem_exec_order_right\n", arguments->boat->id);//todo/////////////////////////////////////////////////////////////////////////////

                sem_wait(&sem_list_exec_order_general); append_boat(list_exec_order_general, arguments->boat); sem_post(&sem_list_exec_order_general);
                sem_wait(&sem_boats_in_canal); boats_in_canal++; sem_post(&sem_boats_in_canal);

                (canal+i_upside_down)->boat=arguments->boat;///Pone el bote en esa posicion
            }
            else///En el medio
            {
                sem_wait(&((canal+i_upside_down)->sem));///Reserva el espacio siguiente
                if((canal+i_upside_down+1)->boat->id == arguments->boat->id)///Si el barco de la posicion anterior es si mismo
                {
                    (canal+i_upside_down+1)->boat=NULL;///Se borra
                }
                (canal+i_upside_down)->boat=arguments->boat;///Pone el bote en la posicion
                sem_post(&((canal+i_upside_down+1)->sem));///Libera el espacio anterior
            }
        }
        usleep((int)((1000000 / arguments->boat->speed) * TIME_FIX_FACTOR));///Simula el tiempo que le toma moverse un espacio
    }

    sem_wait(&sem_boats_in_canal); boats_in_canal--; sem_post(&sem_boats_in_canal);

    if(calendarization_algorithm==0)
    {
        if(arguments->boat->direction==RIGHT)
        {
            sem_wait(&sem_list_RR_left); delete_boat_for_id(list_RR_left, arguments->boat->id); sem_post(&sem_list_RR_left);
        }
        else
        {
            sem_wait(&sem_list_RR_right); delete_boat_for_id(list_RR_right, arguments->boat->id); sem_post(&sem_list_RR_right);
        }
    }

    sem_wait(&sem_direction_aux);///Se apropia de la edicion de los semaforos de direccion
    if(arguments->boat->direction==RIGHT)///Si el barco va para derecha
    {
        ///Baja el numero total contemporaneo de barcos que quedan por pasar desde la izquierda
        sem_wait(&sem_boats_remaining_left); boats_remaining_left--; sem_post(&sem_boats_remaining_left);
    }
    else///Si el barco va para izquierda
    {
        ///Baja el numero total contemporaneo de barcos que quedan por pasar desde la derecha
        sem_wait(&sem_boats_remaining_right); boats_remaining_right--; sem_post(&sem_boats_remaining_right);
    }

    if(arguments->boat->direction==RIGHT)///Si el barco va para la derecha
    {
        printf("A\n");
        if(*(arguments->change_sign_flag) || boats_remaining_left==0)///Time out o no hay mas barcos en la izquierda
        {
            printf("B\n");
            if(*(arguments->change_sign_flag) && boats_remaining_left==0)
            {
                printf("C\n");
                sem_wait(&sem_boats_in_canal);
                if(boats_in_canal==0)///Es el ultimo del canal
                {
                    printf("D\n");
                    if(boats_remaining_right>0)///Hay barcos al otro lado
                    {
                        printf("E\n");
                        ups(&sem_right, canal_length);///Da n permisos a la derecha
                        set_to_zero(&sem_left);
                    }
                    else///No hay barcos en ningun lado
                    {
                        printf("F\n");
                        ups(&sem_left, canal_length);///Reestablece n permisos en la misma direccion
                        set_to_zero(&sem_right);
                    }
                    sem_wait(&(*arguments->sem_change_sign_flag));
                    *(arguments->change_sign_flag)=0;
                    sem_post(&(*arguments->sem_change_sign_flag));
                }
                else///No es el ultimo
                {
                    printf("G\n");
                    ///Nada creo
                }
                sem_post(&sem_boats_in_canal);
            }
            else if(*(arguments->change_sign_flag))///Time out y aun le quedan barcos a la izquierda
            {
                printf("H\n");
                sem_wait(&sem_boats_in_canal);
                if(boats_in_canal==0)///Es el ultimo del canal
                {
                    printf("I\n");
                    if(boats_remaining_right>0)///Hay barcos al otro lado
                    {
                        printf("J\n");
                        ups(&sem_right, canal_length);///Da n permisos a la derecha
                        set_to_zero(&sem_left);
                    }
                    else///Solo hay barcos a la izquierda
                    {
                        printf("K\n");
                        ups(&sem_left, canal_length);///Reestablece n permisos en la misma direccion
                        set_to_zero(&sem_right);
                    }
                    sem_wait(&(*arguments->sem_change_sign_flag));
                    *(arguments->change_sign_flag)=0;
                    sem_post(&(*arguments->sem_change_sign_flag));
                }
                else///No es el ultimo
                {
                    printf("L\n");
                    ///Nada creo
                }
                sem_post(&sem_boats_in_canal);
            }
            else if(boats_remaining_left==0)///No time out pero ya no quedan barcos por pasar en el sentido actual
            {
                printf("M\n");
                if(boats_remaining_right>0)///Aun hay barcos al otro lado
                {
                    printf("N\n");
                    ups(&sem_right, canal_length);///Da n permisos al otro lado
                    set_to_zero(&sem_left);
                }
                else///No hay barcos de ningun lado
                {
                    printf("O\n");
                    ups(&sem_right, canal_length);///Da n permisos a la derecha
                    set_to_zero(&sem_left);
                }
                sem_wait(&(*arguments->sem_change_sign_flag));
                *(arguments->change_sign_flag)=0;
                sem_post(&(*arguments->sem_change_sign_flag));
            }
        }
        else///Si queda tiempo y aun hay barcos a la izquierda
        {
            printf("P\n");
            sem_post(&sem_left);///+1 permisos del mismo lado
        }
    }
    else///Si el barco va para la izquierda
    {
        printf("Q\n");
        if(*(arguments->change_sign_flag) || boats_remaining_right==0)///Time out o no hay mas barcos en la derecha
        {
            printf("R\n");
            if(*(arguments->change_sign_flag) && boats_remaining_right==0)
            {
                printf("S\n");
                sem_wait(&sem_boats_in_canal);
                if(boats_in_canal==0)///Es el ultimo del canal
                {
                    printf("T\n");
                    if(boats_remaining_left>0)///Hay barcos al otro lado
                    {
                        printf("U\n");
                        ups(&sem_left, canal_length);///Da n permisos a la izquierda
                        set_to_zero(&sem_right);
                    }
                    else///No hay barcos en ningun lado
                    {
                        printf("V\n");
                        ups(&sem_left, canal_length);///Reestablece n permisos en la misma direccion
                        set_to_zero(&sem_right);
                    }
                    sem_wait(&(*arguments->sem_change_sign_flag));
                    *(arguments->change_sign_flag)=0;
                    sem_post(&(*arguments->sem_change_sign_flag));
                }
                else///No es el ultimo
                {
                    printf("W\n");
                    ///Nada creo
                }
                sem_post(&sem_boats_in_canal);
            }
            else if(*(arguments->change_sign_flag))///Time out y aun le quedan barcos a la derecha
            {
                printf("X\n");
                sem_wait(&sem_boats_in_canal);
                if(boats_in_canal==0)///Es el ultimo del canal
                {
                    printf("Y\n");
                    if(boats_remaining_left>0)///Hay barcos al otro lado
                    {
                        printf("Z\n");
                        ups(&sem_left, canal_length);///Da n permisos a la izquierda
                        set_to_zero(&sem_right);
                    }
                    else
                    {
                        printf("AA\n");
                        ups(&sem_right, canal_length);///Reestablece n permisos en la misma direccion
                        set_to_zero(&sem_left);
                    }
                    sem_wait(&(*arguments->sem_change_sign_flag));
                    *(arguments->change_sign_flag)=0;
                    sem_post(&(*arguments->sem_change_sign_flag));
                }
                else///No es el ultimo
                {
                    printf("AB\n");
                    ///Nada creo
                }
                sem_post(&sem_boats_in_canal);
            }
            else if(boats_remaining_right==0)///No time out pero ya no quedan barcos por pasar en el sentido actual
            {
                printf("AC\n");
                if(boats_remaining_left>0)///Aun hay barcos al otro lado
                {
                    printf("AD\n");
                    ups(&sem_left, canal_length);///Da n permisos a la izquierda
                    set_to_zero(&sem_right);
                }
                else///No hay barcos de ningun lado
                {
                    printf("AE\n");
                    ups(&sem_left, canal_length);///Da n permisos a la izquierda
                    set_to_zero(&sem_right);
                }
                sem_wait(&(*arguments->sem_change_sign_flag));
                *(arguments->change_sign_flag)=0;
                sem_post(&(*arguments->sem_change_sign_flag));
            }
        }
        else///Si aun queda tiempo y aun hay barcos a la derecha
        {
            printf("AF\n");
            sem_post(&sem_right);///+1 permisos del mismo lado
        }

    }
    sem_post(&sem_direction_aux);
}
void* sign_thread_launcher_func(void* arguments_)
{
    Sign_arguments_launcher_t * arguments=arguments_;
    Boat_Doubly_Linked_List_t* list;
    if(arguments->direction==RIGHT)
    {
        list=list_left;
    }
    else
    {
        list=list_right;
    }
    while(loop)
    {
        if(!is_boat_list_empty(list))
        {
            ///Creating boat thread
            pthread_t thread_boat;
            Sign_arguments_t* sign_arguments = malloc(sizeof(Sign_arguments_t));
            sign_arguments->change_sign_flag=arguments->change_sign_flag;
            sign_arguments->boat=list->first->data;
            sign_arguments->sem_change_sign_flag=arguments->sem_change_sign_flag;

            ///Append creation order list
            sem_wait(&sem_list_creation_order); append_boat(list_creation_order, sign_arguments->boat); sem_post(&sem_list_creation_order);

            ///Exec order & calendarization
            if(sign_arguments->boat->direction==RIGHT)
            {
                sem_wait(&sem_list_exec_order_left); append_boat(list_exec_order_left, sign_arguments->boat); sem_post(&sem_list_exec_order_left);

                ///RR Left
                if(calendarization_algorithm==0)
                {
                    sem_wait(&sem_list_RR_left); append_boat(list_RR_left, sign_arguments->boat); sem_post(&sem_list_RR_left);
                }
                else
                {
                    sem_wait(&sem_schedule_left); schedule(calendarization_algorithm,list_exec_order_left); sem_post(&sem_schedule_left);
                }
            }
            else
            {
                sem_wait(&sem_list_exec_order_right); append_boat(list_exec_order_right, sign_arguments->boat); sem_post(&sem_list_exec_order_right);

                ///RR Right
                if(calendarization_algorithm==0)
                {
                    sem_wait(&sem_list_RR_right); append_boat(list_RR_right, sign_arguments->boat); sem_post(&sem_list_RR_right);
                }
                else
                {
                    sem_wait(&sem_schedule_right); schedule(calendarization_algorithm,list_exec_order_right); sem_post(&sem_schedule_right);
                }
            }

            ///Create thread
            pthread_create(&thread_boat, NULL, sign_thread_func, sign_arguments);
            printf("Created thread for Boat: %d\n", sign_arguments->boat->id);//todo/////////////////////////////////////////////////////////////////////////////

            if(arguments->direction==RIGHT)
            {
                append_thread(thread_list_left, thread_boat);
                sem_wait(&sem_list_left); delete_first(list); sem_post(&sem_list_left);
            }
            else
            {
                append_thread(thread_list_right, thread_boat);
                sem_wait(&sem_list_right); delete_first(list); sem_post(&sem_list_right);
            }
        }
    }
}
void sign()
{
    int* change_sign_flag=malloc(sizeof(int)); *change_sign_flag=0;
    sem_t sem_change_sign_flag;
    sem_init(&sem_change_sign_flag, 0, 1);

    printf("Sign\n");
    printf("Signboard time: %ds\n", signboard_time);

    ///Int for boats count to switch direction semaphores
    int* finished_boats_ptr = malloc(sizeof(int)); *finished_boats_ptr=0;

    ///Semaphores
    if(is_boat_list_empty(list_left))
    {
        sem_init(&sem_left, 0, 0);//Left boats starts bu default
        sem_init(&sem_right, 0, canal_length);
    }
    else
    {
        sem_init(&sem_left, 0, canal_length);//Left boats starts bu default
        sem_init(&sem_right, 0, 0);
    }

    ///Print oceans
    printf("Left:\t"); print_list(list_left);
    printf("Right:\t"); print_list(list_right);

    ///Print semaphores
    printf("Sem Left: "); print_semaphore(&sem_left);
    printf("Sem right: "); print_semaphore(&sem_right);

    boats_remaining_left=get_length(list_left);
    boats_remaining_right=get_length(list_right);

    ///Creating Launcher thread for left boats
    pthread_t thread_launcher_left;
    Sign_arguments_launcher_t* thread_launcher_arguments_left=malloc(sizeof(Sign_arguments_launcher_t));
    thread_launcher_arguments_left->direction=RIGHT;
    thread_launcher_arguments_left->change_sign_flag=change_sign_flag;
    thread_launcher_arguments_left->sem_change_sign_flag=&sem_change_sign_flag;
    pthread_create(&thread_launcher_left, NULL, sign_thread_launcher_func, thread_launcher_arguments_left);

    ///Creating Launcher thread for right boats
    pthread_t thread_launcher_right;
    Sign_arguments_launcher_t* thread_launcher_arguments_right=malloc(sizeof(Sign_arguments_launcher_t));
    thread_launcher_arguments_right->direction=LEFT;
    thread_launcher_arguments_right->change_sign_flag=change_sign_flag;
    thread_launcher_arguments_right->sem_change_sign_flag=&sem_change_sign_flag;
    pthread_create(&thread_launcher_right, NULL, sign_thread_launcher_func, thread_launcher_arguments_right);

    ///Creating threads for RR control
    if(calendarization_algorithm==0)
    {
        pthread_t RR_thread_left;
        Tico_arguments_launcher_t* RR_arguments_left=malloc(sizeof(Tico_arguments_launcher_t));
        RR_arguments_left->direction=RIGHT;
        pthread_create(&RR_thread_left, NULL, RR_func, RR_arguments_left);

        pthread_t RR_thread_right;
        Tico_arguments_launcher_t* RR_arguments_right=malloc(sizeof(Tico_arguments_launcher_t));
        RR_arguments_right->direction=LEFT;
        pthread_create(&RR_thread_right, NULL, RR_func, RR_arguments_right);
    }


    while(loop)
    {
        if((*change_sign_flag)==1)
        {
            ///Busy waiting
        }
        else if((boats_remaining_left!=0 || boats_remaining_right!=0) && boats_in_canal!=0)
        {
            sleep(signboard_time);
            sem_wait(&sem_change_sign_flag);
            (*change_sign_flag)=1;
            printf("[\t[\tTime out\t]\t]\n");
            sem_post(&sem_change_sign_flag);
        }
    }

    ///Waiting for launcher threads
    pthread_join(thread_launcher_left, NULL);
    pthread_join(thread_launcher_right, NULL);
}
void* tico_thread_func(void* arguments_)
{
    ///adquiere los parametros
    Tico_arguments_t* arguments = arguments_;


    ///Direction semaphores
    if(arguments->boat->direction==RIGHT)
    {
        sem_wait(&sem_direction_aux);///Se apropia de la edicion de los semaforos de direccion
        int sem_value; sem_getvalue(&sem_left, &sem_value);
        if(boats_in_canal==0 && sem_value==0 && boats_remaining_right==0)
        {
            ups(&sem_left, canal_length);
            set_to_zero(&sem_right);
        }
        sem_post(&sem_direction_aux);///Termina de apropiarce de la edicion de los semaforos de direccion

        ///Busy waiting hasta que le toque su turno
        while((list_exec_order_left->first->data->id!=arguments->boat->id)){/*Busy waiting equis de*/}
        sem_wait(&sem_left);///Se come un permiso de entrar por la izquierda
    }
    else
    {
        sem_wait(&sem_direction_aux);///Se apropia de la edicion de los semaforos de direccion
        int sem_value; sem_getvalue(&sem_right, &sem_value);
        if(boats_in_canal==0 && sem_value==0 && boats_remaining_left==0)
        {
            ups(&sem_right, canal_length);
            set_to_zero(&sem_left);

            //sem_wait(&(*arguments->sem_change_sign_flag));
            //*(arguments->change_sign_flag)=0;
            //sem_post(&(*arguments->sem_change_sign_flag));
        }
        sem_post(&sem_direction_aux);///Se apropia de la edicion de los semaforos de direccion

        //sem_wait(&sem_schedule_right); schedule(calendarization_algorithm,list_exec_order_right); sem_post(&sem_schedule_right);

        while(list_exec_order_right->first->data->id!=arguments->boat->id)///Busy waiting hasta que le toque su turno
        {/*Busy waiting equis de*/}
        //printf("%d antes de sem_right\n", arguments->boat->id);//todo/////////////////////////////////////////////////////////////////////////////
        sem_wait(&sem_right);///Se come un permiso de entrar por la derecha
        //printf("%d despues de sem_right\n", arguments->boat->id);//todo/////////////////////////////////////////////////////////////////////////////
    }

    int RR_aux=1;

    ///Recorre el canal
    for(int i=0; i<=canal_length; i++)
    {
        if(arguments->boat->direction==RIGHT)///Si el barco va para la derecha
        {
            if(calendarization_algorithm==0)
            {
                if(!is_boat_list_empty(list_RR_left))
                {
                    while(RR_aux)
                    {
                        sem_wait(&sem_list_RR_left);
                        if(list_RR_left->first->data->id==arguments->boat->id)
                        {
                            RR_aux=0;
                        }
                        sem_post(&sem_list_RR_left);
                    }
                    printf("%d moves in quantum\n", arguments->boat->id);
                    RR_aux=1;
                }
            }

            arguments->boat->position=i;///Actualiza la posicion del barco
            if(i==canal_length)///Final
            {
                if((canal+i-1)->boat->id == arguments->boat->id)///Si el barco de la posicion anterior es si mismo
                {
                    (canal+i-1)->boat=NULL;///Se borra
                }
                printf("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tSale: %d\n", arguments->boat->id);//todo/////////////////////////////////////////////////////////////////////////////
                sem_post(&((canal+i-1)->sem));///Libera el espacio anterior
            }
            else if(i==0)///Inicio
            {
                //while(exec_order_left->first->data->id!=arguments->boat->id)///Busy waiting hasta que le toque su turno
                //{/*Busy waiting equis de*/}
                //printf("%d antes de sem: %d\n", arguments->boat->id, i);//todo/////////////////////////////////////////////////////////////////////////////
                sem_wait(&((canal+i)->sem));///Reserva el espacio siguiente
                //printf("%d despues de sem: %d\n", arguments->boat->id, i);//todo/////////////////////////////////////////////////////////////////////////////

                printf("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tEntra: %d\n", arguments->boat->id);

                //printf("%d antes de sem_exec_order_left\n", arguments->boat->id);//todo/////////////////////////////////////////////////////////////////////////////
                sem_wait(&sem_list_exec_order_left);
                delete_first(list_exec_order_left);
                sem_post(&sem_list_exec_order_left);
                //printf("%d despues de sem_exec_order_left\n", arguments->boat->id);//todo/////////////////////////////////////////////////////////////////////////////

                sem_wait(&sem_list_exec_order_general); append_boat(list_exec_order_general, arguments->boat); sem_post(&sem_list_exec_order_general);
                sem_wait(&sem_boats_in_canal); boats_in_canal++; sem_post(&sem_boats_in_canal);

                (canal+i)->boat=arguments->boat;///Pone el bote en esa posicion
            }
            else///En el medio
            {
                sem_wait(&((canal+i)->sem));///Reserva el espacio siguiente
                if((canal+i-1)->boat->id == arguments->boat->id)///Si el barco de la posicion anterior es si mismo
                {
                    (canal+i-1)->boat=NULL;///Se borra
                }
                (canal+i)->boat=arguments->boat;///Pone el bote en la posicion
                sem_post(&((canal+i-1)->sem));///Libera el espacio anterior
            }
        }
        else///Si el barco va para la izquierda
        {
            if(calendarization_algorithm==0)
            {
                if(!is_boat_list_empty(list_RR_right))
                {
                    while(RR_aux)
                    {
                        sem_wait(&sem_list_RR_right);
                        if(list_RR_right->first->data->id==arguments->boat->id)
                        {
                            RR_aux=0;
                        }
                        sem_post(&sem_list_RR_right);
                    }
                    printf("%d moves in quantum\n", arguments->boat->id);
                    RR_aux=1;
                }
            }

            int i_upside_down=(canal_length-i-1);///i de derecha a izquierda (i al reves)
            arguments->boat->position=i_upside_down;///Actualiza la posicion del barco

            if(i==canal_length)///Final
            {
                if((canal+i_upside_down+1)->boat->id == arguments->boat->id)///Si el barco de la posicion anterior es si mismo
                {
                    (canal+i_upside_down+1)->boat=NULL;///Se borra
                }
                printf("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tSale: %d\n", arguments->boat->id);//todo/////////////////////////////////////////////////////////////////////////////
                sem_post(&((canal+i_upside_down+1)->sem));///Libera el espacio anterior
            }
            else if(i==0)///Inicio
            {
                //while(exec_order_right->first->data->id!=arguments->boat->id)///Busy waiting hasta que le toque su turno
                //{/*Busy waiting equis de*/}
                //printf("%d antes de sem: %d\n", arguments->boat->id, i_upside_down);//todo/////////////////////////////////////////////////////////////////////////////
                sem_wait(&((canal+i_upside_down)->sem));///Reserva el espacio siguiente
                //printf("%d despues de sem: %d\n", arguments->boat->id, i_upside_down);//todo/////////////////////////////////////////////////////////////////////////////

                printf("\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\tEntra: %d\n", arguments->boat->id);

                //printf("%d antes de sem_exec_order_right\n", arguments->boat->id);//todo/////////////////////////////////////////////////////////////////////////////
                sem_wait(&sem_list_exec_order_right);
                delete_first(list_exec_order_right);
                sem_post(&sem_list_exec_order_right);
                //printf("%d despues de sem_exec_order_right\n", arguments->boat->id);//todo/////////////////////////////////////////////////////////////////////////////

                sem_wait(&sem_list_exec_order_general); append_boat(list_exec_order_general, arguments->boat); sem_post(&sem_list_exec_order_general);
                sem_wait(&sem_boats_in_canal); boats_in_canal++; sem_post(&sem_boats_in_canal);

                (canal+i_upside_down)->boat=arguments->boat;///Pone el bote en esa posicion
            }
            else///En el medio
            {
                sem_wait(&((canal+i_upside_down)->sem));///Reserva el espacio siguiente
                if((canal+i_upside_down+1)->boat->id == arguments->boat->id)///Si el barco de la posicion anterior es si mismo
                {
                    (canal+i_upside_down+1)->boat=NULL;///Se borra
                }
                (canal+i_upside_down)->boat=arguments->boat;///Pone el bote en la posicion
                sem_post(&((canal+i_upside_down+1)->sem));///Libera el espacio anterior
            }
        }
        //printf("step\n");//todo/////////////////////////////////////////////////////////////////////////////
        //printf("\t\t\t\t\t %d\n", (int)((1000000 / arguments->boat->speed) * TIME_FIX_FACTOR));
        usleep((int)((1000000 / arguments->boat->speed) * TIME_FIX_FACTOR));///Simula el tiempo que le toma moverse un espacio
    }


    sem_wait(&sem_boats_in_canal); boats_in_canal--; sem_post(&sem_boats_in_canal);
    //printf("\n");//todo/////////////////////////////////////////////////////////////////////////////


    ///RR deletes
    if(calendarization_algorithm==0)
    {
        if(arguments->boat->direction==RIGHT)
        {
            sem_wait(&sem_list_RR_left); delete_boat_for_id(list_RR_left, arguments->boat->id); sem_post(&sem_list_RR_left);
        }
        else
        {
            sem_wait(&sem_list_RR_right); delete_boat_for_id(list_RR_right, arguments->boat->id); sem_post(&sem_list_RR_right);
        }
    }


    sem_wait(&sem_direction_aux);///Se apropia de la edicion de los semaforos de direccion
    if(arguments->boat->direction==RIGHT)///Si el barco va para derecha
    {
        ///Baja el numero total contemporaneo de barcos que quedan por pasar desde la izquierda
        sem_wait(&sem_boats_remaining_left); boats_remaining_left--; sem_post(&sem_boats_remaining_left);
    }
    else///Si el barco va para izquierda
    {
        ///Baja el numero total contemporaneo de barcos que quedan por pasar desde la derecha
        sem_wait(&sem_boats_remaining_right); boats_remaining_right--; sem_post(&sem_boats_remaining_right);
    }

    if(arguments->boat->direction==RIGHT)///El barco va para la derecha
    {
        printf("A\n");
        if(boats_remaining_left==0)///No hay mas barcos en la izquierda
        {
            printf("B\n");
            //sem_wait(&sem_boats_in_canal);
            if(boats_in_canal==0)///Es el ultimo del canal
            {
                printf("C\n");
                if(boats_remaining_right>0)///Hay barcos al otro lado
                {
                    printf("D\n");
                    ups(&sem_right, canal_length);///Da n permisos a la derecha
                    set_to_zero(&sem_left);
                }
                else///No hay barcos en ningun lado
                {
                    printf("E\n");
                    ups(&sem_left, canal_length);///Reestablece n permisos en la misma direccion
                    set_to_zero(&sem_right);
                }
            }
            else///No es el ultimo
            {
                printf("F\n");
                ///Nada creo
            }
            //sem_post(&sem_boats_in_canal);
        }
        else///Aun hay barcos a la izquierda
        {
            printf("G\n");
            sem_post(&sem_left);///+1 permisos del mismo lado
        }
    }
    else///Si el barco va para la izquierda
    {
        printf("H\n");
        if(boats_remaining_right==0)///No hay mas barcos en la right
        {
            printf("I\n");
            //sem_wait(&sem_boats_in_canal);
            if(boats_in_canal==0)///Es el ultimo del canal
            {
                printf("J\n");
                if(boats_remaining_left>0)///Hay barcos al otro lado
                {
                    printf("K\n");
                    ups(&sem_left, canal_length);///Da n permisos a la derecha
                    set_to_zero(&sem_right);
                }
                else///No hay barcos en ningun lado
                {
                    printf("L\n");
                    ups(&sem_right, canal_length);///Reestablece n permisos en la misma direccion
                    set_to_zero(&sem_left);
                }
            }
            else///No es el ultimo
            {
                printf("M\n");
                ///Nada creo
            }
            //sem_post(&sem_boats_in_canal);
        }
        else///Aun hay barcos a la derecha
        {
            printf("N\n");
            sem_post(&sem_right);///+1 permisos del mismo lado
        }
    }
    sem_post(&sem_direction_aux);
}
void* tico_thread_launcher_func(void* arguments_)
{
    Tico_arguments_launcher_t * arguments=arguments_;
    Boat_Doubly_Linked_List_t* list;
    if(arguments->direction==RIGHT)
    {
        list=list_left;
    }
    else
    {
        list=list_right;
    }
    while(loop)//todo hacer q otro thread cambie una variable booleana global <continue> que cambie con un getchar()
        //while(1)
    {
        if(!is_boat_list_empty(list))
        {
            ///Creating boat thread
            pthread_t thread_boat;
            Tico_arguments_t* tico_arguments = malloc(sizeof(Tico_arguments_t));
            tico_arguments->boat=list->first->data;

            //printf("Thread: %d antes de sem_create_order\n", arguments->direction);//todo/////////////////////////////////////////////////////////////////////////////
            sem_wait(&sem_list_creation_order);
            append_boat(list_creation_order, tico_arguments->boat);
            sem_post(&sem_list_creation_order);
            //printf("Thread: %d despues de sem_create_order\n", arguments->direction);//todo/////////////////////////////////////////////////////////////////////////////

            if(tico_arguments->boat->direction==RIGHT)
            {
                //printf("Thread: %d antes de sem_exec_order_left\n", arguments->direction);//todo/////////////////////////////////////////////////////////////////////////////
                sem_wait(&sem_list_exec_order_left); append_boat(list_exec_order_left, tico_arguments->boat); sem_post(&sem_list_exec_order_left);

                //printf("Thread: %d despues de sem_exec_order_left\n", arguments->direction);//todo/////////////////////////////////////////////////////////////////////////////
                if(calendarization_algorithm==0)
                {
                    sem_wait(&sem_list_RR_left); append_boat(list_RR_left, tico_arguments->boat); sem_post(&sem_list_RR_left);
                }
                else
                {
                    sem_wait(&sem_schedule_left); schedule(calendarization_algorithm,list_exec_order_left); sem_post(&sem_schedule_left);
                }
            }
            else
            {
                //printf("Thread: %d antes de sem_exec_order_right\n", arguments->direction);//todo/////////////////////////////////////////////////////////////////////////////
                sem_wait(&sem_list_exec_order_right); append_boat(list_exec_order_right, tico_arguments->boat); sem_post(&sem_list_exec_order_right);

                //printf("Thread: %d despues de sem_exec_order_right\n", arguments->direction);//todo/////////////////////////////////////////////////////////////////////////////
                if(calendarization_algorithm==0)
                {
                    sem_wait(&sem_list_RR_right); append_boat(list_RR_right, tico_arguments->boat); sem_post(&sem_list_RR_right);
                }
                else
                {
                    sem_wait(&sem_schedule_right); schedule(calendarization_algorithm,list_exec_order_right); sem_post(&sem_schedule_right);
                }
            }

            ///Create thread
            pthread_create(&thread_boat, NULL, tico_thread_func, tico_arguments);

            if(arguments->direction==RIGHT)
            {
                append_thread(thread_list_left, thread_boat);

                //printf("Thread: %d antes de sem_list_left\n", arguments->direction);//todo/////////////////////////////////////////////////////////////////////////////
                sem_wait(&sem_list_left);
                delete_first(list);//todo no borrar nodos para poder ordenar la lista con el calendarizacion luego
                sem_post(&sem_list_left);
                //printf("Thread: %d despues de sem_list_left\n", arguments->direction);//todo/////////////////////////////////////////////////////////////////////////////
            }
            else
            {
                append_thread(thread_list_right, thread_boat);

                //printf("Thread: %d antes de sem_list_right\n", arguments->direction);//todo/////////////////////////////////////////////////////////////////////////////
                sem_wait(&sem_list_right);
                delete_first(list);//todo no borrar nodos para poder ordenar la lista con el calendarizacion luego
                sem_post(&sem_list_right);
                //printf("Thread: %d despues de sem_list_right\n", arguments->direction);//todo/////////////////////////////////////////////////////////////////////////////
            }
            printf("Created thread for Boat: %d\n", tico_arguments->boat->id);//todo/////////////////////////////////////////////////////////////////////////////
        }
        else//If empty list
        {
            ///Wait for boat threads
        }
    }
}
void tico()
{
    printf("Tico\n");


    ///Int for boats count to switch direction semaphores
    int* finished_boats_ptr = malloc(sizeof(int)); *finished_boats_ptr=0;


    ///Semaphores
    if(is_boat_list_empty(list_left))
    {
        sem_init(&sem_left, 0, 0);//Left boats starts bu default
        sem_init(&sem_right, 0, canal_length);
    }
    else
    {
        sem_init(&sem_left, 0, canal_length);//Left boats starts bu default
        sem_init(&sem_right, 0, 0);
    }


    ///Print oceans
    printf("Left:\t"); print_list(list_left);
    printf("Right:\t"); print_list(list_right);


    ///Print semaphores
    printf("Sem Left: "); print_semaphore(&sem_left);
    printf("Sem right: "); print_semaphore(&sem_right);

    boats_remaining_left=get_length(list_left);
    boats_remaining_right=get_length(list_right);


    ///Creating Launcher thread for left boats
    pthread_t thread_launcher_left;
    Tico_arguments_launcher_t* thread_launcher_arguments_left=malloc(sizeof(Tico_arguments_launcher_t));
    thread_launcher_arguments_left->direction=RIGHT;
    pthread_create(&thread_launcher_left, NULL, tico_thread_launcher_func, thread_launcher_arguments_left);


    ///Creating Launcher thread for right boats
    pthread_t thread_launcher_right;
    Tico_arguments_launcher_t* thread_launcher_arguments_right=malloc(sizeof(Tico_arguments_launcher_t));
    thread_launcher_arguments_right->direction=LEFT;
    pthread_create(&thread_launcher_right, NULL, tico_thread_launcher_func, thread_launcher_arguments_right);

    if(calendarization_algorithm==0)
    {
        pthread_t RR_thread_left;
        Tico_arguments_launcher_t* RR_arguments_left=malloc(sizeof(Tico_arguments_launcher_t));
        RR_arguments_left->direction=RIGHT;
        pthread_create(&RR_thread_left, NULL, RR_func, RR_arguments_left);

        pthread_t RR_thread_right;
        Tico_arguments_launcher_t* RR_arguments_right=malloc(sizeof(Tico_arguments_launcher_t));
        RR_arguments_right->direction=LEFT;
        pthread_create(&RR_thread_right, NULL, RR_func, RR_arguments_right);

        pthread_join(RR_thread_left, NULL);
        pthread_join(RR_thread_right, NULL);
    }

    ///Waiting for launcher threads
    pthread_join(thread_launcher_left, NULL);
    pthread_join(thread_launcher_right, NULL);
}


int main()
{
    ///Leer valores del file
    int w;
    Load_t* load = malloc(sizeof(Load_t));
    FILE *fp;
    char buffer[1024];
    struct json_object *parsedJson;
    struct json_object *SchedulerAlgorithm;
    struct json_object *FlowControlMethod;
    struct json_object *ChannelLength;
    struct json_object *BoatSpeed;
    struct json_object *BoatQuantity;
    struct json_object *WParameter;
    struct json_object *Signboard;
    struct json_object *DefinedLoadLeft;
    struct json_object *DefinedLoadRight;
    struct json_object *NormalBoat;
    struct json_object *FishingBoat;
    struct json_object *PatrolBoat;
    struct json_object *Priority;
    struct json_object *EDF;

    size_t loadSize;
    size_t i;

    fp = fopen("config.json", "r");
    fread(buffer, 1024, 1, fp);
    fclose(fp);

    parsedJson = json_tokener_parse(buffer);

    json_object_object_get_ex(parsedJson, "SchedulerAlgorithm", &SchedulerAlgorithm);
    json_object_object_get_ex(parsedJson, "FlowControlMethod", &FlowControlMethod);
    json_object_object_get_ex(parsedJson, "ChannelLength", &ChannelLength);
    json_object_object_get_ex(parsedJson, "BoatSpeed", &BoatSpeed);
    json_object_object_get_ex(parsedJson, "BoatQuantity", &BoatQuantity);
    json_object_object_get_ex(parsedJson, "WParameter", &WParameter);
    json_object_object_get_ex(parsedJson, "Signboard", &Signboard);
    json_object_object_get_ex(parsedJson, "DefinedLoadLeft", &DefinedLoadLeft);
    json_object_object_get_ex(parsedJson, "DefinedLoadRight", &DefinedLoadRight);
    json_object_object_get_ex(parsedJson, "Priority", &Priority);
    json_object_object_get_ex(parsedJson, "EDF", &EDF);

    calendarization_algorithm = json_object_get_int(SchedulerAlgorithm);
    canal_algorithm = json_object_get_int(FlowControlMethod);
    canal_length = json_object_get_int(ChannelLength);
    base_speed = json_object_get_int(BoatSpeed);
    boat_quantity = json_object_get_int(BoatQuantity);
    w = json_object_get_int(WParameter);
    signboard_time = json_object_get_int(Signboard);

    load -> left[0] = json_object_get_int(json_object_array_get_idx(DefinedLoadLeft, 0));
    load -> left[1] = json_object_get_int(json_object_array_get_idx(DefinedLoadLeft, 1));
    load -> left[2] = json_object_get_int(json_object_array_get_idx(DefinedLoadLeft, 2));
        load -> right[0] = json_object_get_int(json_object_array_get_idx(DefinedLoadRight, 0));
        load -> right[1] = json_object_get_int(json_object_array_get_idx(DefinedLoadRight, 1));
        load -> right[2] = json_object_get_int(json_object_array_get_idx(DefinedLoadRight, 2));

    priority[0] = json_object_get_int(json_object_array_get_idx(Priority, 0));
    priority[1] = json_object_get_int(json_object_array_get_idx(Priority, 1));

    edf_deadlines[0] = json_object_get_int(json_object_array_get_idx(EDF, 0));
    edf_deadlines[1] = json_object_get_int(json_object_array_get_idx(EDF, 1));
    edf_deadlines[2] = json_object_get_int(json_object_array_get_idx(EDF, 2));

    ///Prints generales
    printf("Scheduler algorithm: ");
    switch(calendarization_algorithm)
    {
        case 0:  printf("Round Robin\n"); break;
        case 1:  printf("Priority\n"); break;
        case 2:  printf("SJF\n"); break;
        case 3:  printf("FCFS\n"); break;
        case 4:  printf("EDF\n"); break;
        default:  printf("Invalid scheduler algorithm\n"); break;
    }
    printf("Flow control method: ");
    switch(canal_algorithm)
    {
        case 0:  printf("Equity\n"); break;
        case 1:  printf("Sign\n"); break;
        case 2:  printf("Tico\n"); break;
        default:  printf("Invalid flow control method\n"); break;
    }

    ///Crea las estructuras de control
    list_left = create_boat_list(list_left);
    sem_init(&sem_list_left, 0, 1);

    list_right = create_boat_list(list_right);
    sem_init(&sem_list_right, 0, 1);

    sem_init(&sem_direction_aux, 0, 1);

    list_exec_order_general = create_boat_list(list_exec_order_general);
    sem_init(&sem_list_exec_order_general, 0, 1);

    list_exec_order_left = create_boat_list(list_exec_order_left);
    sem_init(&sem_list_exec_order_left, 0, 1);

    list_exec_order_right = create_boat_list(list_exec_order_right);
    sem_init(&sem_list_exec_order_right, 0, 1);

    list_creation_order = create_boat_list(list_creation_order);
    sem_init(&sem_list_creation_order, 0, 1);

    sem_init(&sem_boats_remaining_left, 0, 1);
    sem_init(&sem_boats_remaining_right, 0, 1);

    thread_list_left=create_thread_list(thread_list_left);
    thread_list_right=create_thread_list(thread_list_right);

    sem_init(&sem_schedule_left, 0, 1);
    sem_init(&sem_schedule_right, 0, 1);

    sem_init(&sem_boats_in_canal, 0, 1);

    if(calendarization_algorithm==0)
    {
        list_RR_left = create_boat_list(list_RR_left);
        sem_init(&sem_list_RR_left, 0, 1);

        list_RR_right = create_boat_list(list_RR_right);
        sem_init(&sem_list_RR_right, 0, 1);
    }


    ///Crea el canal
    canal_init();


    ///Carga los barcos en las listas
    initial_load(load);


    ///El calendarizador ordena 1 vez inicialmente si las listas tienen carga inicial
    if(!is_boat_list_empty(list_left))
    {
        schedule(calendarization_algorithm, list_left);
        //print_complete_list(list_left);
        //print_list_priority(list_left);
        //print_list_speed(list_left);
    }
    if(!is_boat_list_empty(list_right))
    {
        schedule(calendarization_algorithm, list_right);
        //print_list_priority(list_right);
        //print_list_speed(list_right);
    }

    pthread_t boat_spawner_thread;
    pthread_create(&boat_spawner_thread, NULL, boat_spawner_func, NULL);

    switch(canal_algorithm)
    {
        case 0: equity(w); break;
        case 1: sign(); break;
        case 2: tico(); break;
        default: printf("Invalid flow control method\n");
    }

    pthread_join(boat_spawner_thread, NULL);

    ///Prints
    printf("\n");
    printf("Creation order: "); print_list(list_creation_order);
    printf("Execution order: "); print_list(list_exec_order_general);
    print_canal();
    printf("Sem Left: "); print_semaphore(&sem_left);
    printf("Sem right: "); print_semaphore(&sem_right);

    printf("Execution order left: "); print_list(list_exec_order_left);
    printf("Execution order right: "); print_list(list_exec_order_right);


    return 0;
}