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
        normal_boat->id=get_id(); normal_boat->type=0; normal_boat->position=0; normal_boat->direction=RIGHT; normal_boat->speed=base_speed; normal_boat->life_time=LIFE_TIME_IMMORTAL; normal_boat->priority=10; //normal_boat->arrival_time=0;
        append_boat(list_left, normal_boat);
        load->left[0]--;
    }
    while(load->left[1] > 0)
    {
        Boat_t* fisher_boat = malloc(sizeof(Boat_t));
        fisher_boat->id=get_id(); fisher_boat->type=1; fisher_boat->position=0; fisher_boat->direction=RIGHT; fisher_boat->speed=base_speed*FISHER_BOAT_SPEED_MULTIPLIER;fisher_boat->life_time=LIFE_TIME_IMMORTAL; fisher_boat->priority=20; //fisher_boat->arrival_time=0;
        append_boat(list_left, fisher_boat);
        load->left[1]--;
    }
    while(load->left[2] > 0)
    {
        Boat_t* patrol_boat = malloc(sizeof(Boat_t));
        patrol_boat->id=get_id(); patrol_boat->type=2; patrol_boat->position=0; patrol_boat->direction=RIGHT; patrol_boat->speed=base_speed*PATROL_BOAT_SPEED_MULTIPLIER; patrol_boat->life_time=LIFE_TIME_STANDARD; patrol_boat->priority=15;
        append_boat(list_left, patrol_boat);
        load->left[2]--;
    }

    while(load->right[0] > 0)
    {
        Boat_t* normal_boat = malloc(sizeof(Boat_t));
        normal_boat->id=get_id(); normal_boat->type=0; normal_boat->position=0; normal_boat->direction=LEFT; normal_boat->speed=base_speed; normal_boat->life_time=LIFE_TIME_IMMORTAL; normal_boat->priority=20;
        append_boat(list_right, normal_boat);
        load->right[0]--;
    }
    while(load->right[1] > 0)
    {
        Boat_t* fisher_boat = malloc(sizeof(Boat_t));
        fisher_boat->id=get_id(); fisher_boat->type=1; fisher_boat->position=0; fisher_boat->direction=LEFT; fisher_boat->speed=base_speed*FISHER_BOAT_SPEED_MULTIPLIER;fisher_boat->life_time=LIFE_TIME_IMMORTAL; fisher_boat->priority=10;
        append_boat(list_right, fisher_boat);
        load->right[1]--;
    }
    while(load->right[2] > 0)
    {
        Boat_t* patrol_boat = malloc(sizeof(Boat_t));
        patrol_boat->id=get_id(); patrol_boat->type=2; patrol_boat->position=0; patrol_boat->direction=LEFT; patrol_boat->speed=base_speed*PATROL_BOAT_SPEED_MULTIPLIER; patrol_boat->life_time=LIFE_TIME_STANDARD; patrol_boat->priority=20;
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
            printBoat((canal+i)->boat);
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
            case 'q':///Patrol Left
                boat->id=get_id(); boat->type=2; boat->position=0; boat->direction=RIGHT; boat->speed=base_speed*PATROL_BOAT_SPEED_MULTIPLIER; boat->life_time=LIFE_TIME_STANDARD; boat->priority=2;
                sem_wait(&sem_list_left); append_boat(list_left, boat); sem_post(&sem_list_left);
                printf("Patrol Boat: %d spawns on left\n", boat->id);
                sem_wait(&sem_boats_remaining_left); boats_remaining_left++; sem_post(&sem_boats_remaining_left);
                break;
            case 'w':///Fisher Left
                boat->id=get_id(); boat->type=1; boat->position=0; boat->direction=RIGHT; boat->speed=base_speed*FISHER_BOAT_SPEED_MULTIPLIER; boat->life_time=LIFE_TIME_IMMORTAL; boat->priority=1;
                sem_wait(&sem_list_left); append_boat(list_left, boat); sem_post(&sem_list_left);
                printf("Fisher Boat: %d spawns on left\n", boat->id);
                sem_wait(&sem_boats_remaining_left); boats_remaining_left++; sem_post(&sem_boats_remaining_left);
                break;
            case 'e':///Normal Left
                boat->id=get_id(); boat->type=0; boat->position=0; boat->direction=RIGHT; boat->speed=base_speed; boat->life_time=LIFE_TIME_IMMORTAL; boat->priority=0;
                sem_wait(&sem_list_left); append_boat(list_left, boat); sem_post(&sem_list_left);
                printf("Normal Boat: %d spawns on left\n", boat->id);
                sem_wait(&sem_boats_remaining_left); boats_remaining_left++; sem_post(&sem_boats_remaining_left);
                break;
            case 'r':///Normal Right
                boat->id=get_id(); boat->type=0; boat->position=0; boat->direction=LEFT; boat->speed=base_speed; boat->life_time=LIFE_TIME_IMMORTAL; boat->priority=0;
                sem_wait(&sem_list_right); append_boat(list_right, boat); sem_post(&sem_list_right);
                printf("Normal Boat: %d spawns on right\n", boat->id);
                sem_wait(&sem_boats_remaining_right); boats_remaining_right++; sem_post(&sem_boats_remaining_right);
                break;
            case 't':///Fisher Right
                boat->id=get_id(); boat->type=1; boat->position=0; boat->direction=LEFT; boat->speed=base_speed*FISHER_BOAT_SPEED_MULTIPLIER; boat->life_time=LIFE_TIME_IMMORTAL; boat->priority=1;
                sem_wait(&sem_list_right); append_boat(list_right, boat); sem_post(&sem_list_right);
                printf("Fisher Boat: %d spawns on right\n", boat->id);
                sem_wait(&sem_boats_remaining_right); boats_remaining_right++; sem_post(&sem_boats_remaining_right);
                break;
            case 'y':///Patrol Right
                boat->id=get_id(); boat->type=2; boat->position=0; boat->direction=LEFT; boat->speed=base_speed*PATROL_BOAT_SPEED_MULTIPLIER; boat->life_time=LIFE_TIME_STANDARD; boat->priority=2;
                sem_wait(&sem_list_right); append_boat(list_right, boat); sem_post(&sem_list_right);
                printf("Patrol Boat: %d spawns on right\n", boat->id);
                sem_wait(&sem_boats_remaining_right); boats_remaining_right++; sem_post(&sem_boats_remaining_right);
                break;

            default:
                if(caracter==' ')
                {
                    loop=0;
                };
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
        //sem_wait(&sem_schedule_left); schedule(calendarization_algorithm,list_exec_order_left); sem_post(&sem_schedule_left);

        while(list_exec_order_left->first->data->id!=arguments->boat->id)///Busy waiting hasta que le toque su turno
        {/*Busy waiting equis de*/}
        //printf("%d antes de sem_left\n", arguments->boat->id);//todo/////////////////////////////////////////////////////////////////////////////
        sem_wait(&sem_left);///Se come un permiso de entrar por la izquierda
        //printf("%d despues de sem_left\n", arguments->boat->id);//todo/////////////////////////////////////////////////////////////////////////////
    }
    else
    {
        //sem_wait(&sem_schedule_right); schedule(calendarization_algorithm,list_exec_order_right); sem_post(&sem_schedule_right);

        while(list_exec_order_right->first->data->id!=arguments->boat->id)///Busy waiting hasta que le toque su turno
        {/*Busy waiting equis de*/}
        //printf("%d antes de sem_right\n", arguments->boat->id);//todo/////////////////////////////////////////////////////////////////////////////
        sem_wait(&sem_right);///Se come un permiso de entrar por la derecha
        //printf("%d despues de sem_right\n", arguments->boat->id);//todo/////////////////////////////////////////////////////////////////////////////
    }

    ///Recorre el canal
    for(int i=0; i<=canal_length; i++)
    {
        if(arguments->boat->direction==RIGHT)///Si el barco va para la derecha
        {
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
    //printf("\n");//todo/////////////////////////////////////////////////////////////////////////////

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
        if(*(arguments->finished_boats)==arguments->w-1 || boats_remaining_left==0)///Si ya pasaron los w barcos o no hay mas barcos que pasar desde la izquierda
        {
            if(boats_remaining_right==0)///Si no hay mas barcos que pasar desde la derecha
            {
                ups(&sem_left, arguments->w);///Reestablece el semaforo de la izquierda pq no hay barcos a la derecha
                set_to_zero(&sem_right);
            }
            else
            {
                ups(&sem_right, arguments->w);///Reestablece el semaforo de la derecha pq hay que cambiar de sentido
                set_to_zero(&sem_left);
            }
            *(arguments->finished_boats)=0;///Reestablece la cantidad de barcos por sentido
        }
        else///Si no han pasado w barcos y hay barcos a la derecha que quieren pasar
        {
            *(arguments->finished_boats)=*(arguments->finished_boats)+1;///Aumenta la cantidad de barcos q han pasado desde la izquierda
        }
    }
    else///Si el barco va para la izquierda
    {
        if(*(arguments->finished_boats)==arguments->w-1 || boats_remaining_right==0)
        {
            if(boats_remaining_left==0)
            {
                ups(&sem_right, arguments->w);
                set_to_zero(&sem_left);
            }
            else
            {
                ups(&sem_left, arguments->w);
                set_to_zero(&sem_right);
            }
            *(arguments->finished_boats)=0;
        }
        else
        {
            *(arguments->finished_boats)=*(arguments->finished_boats)+1;
            ///*(finished_boats)++; //todo WHY NOT?
        }
    }

    sem_post(&sem_direction_aux);
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
            equity_arguments->finished_boats=arguments->finished_boats_ptr;
            equity_arguments->boat=list->first->data;

            //printf("Thread: %d antes de sem_create_order\n", arguments->direction);//todo/////////////////////////////////////////////////////////////////////////////
            sem_wait(&sem_list_creation_order);
                append_boat(list_creation_order, equity_arguments->boat);
            sem_post(&sem_list_creation_order);
            //printf("Thread: %d despues de sem_create_order\n", arguments->direction);//todo/////////////////////////////////////////////////////////////////////////////

            if(equity_arguments->boat->direction==RIGHT)
            {
                //printf("Thread: %d antes de sem_exec_order_left\n", arguments->direction);//todo/////////////////////////////////////////////////////////////////////////////
                sem_wait(&sem_list_exec_order_left);
                    append_boat(list_exec_order_left, equity_arguments->boat);
                    sem_wait(&sem_schedule_left); schedule(calendarization_algorithm,list_exec_order_left); sem_post(&sem_schedule_left);
                sem_post(&sem_list_exec_order_left);
                //printf("Thread: %d despues de sem_exec_order_left\n", arguments->direction);//todo/////////////////////////////////////////////////////////////////////////////
            }
            else
            {
                //printf("Thread: %d antes de sem_exec_order_right\n", arguments->direction);//todo/////////////////////////////////////////////////////////////////////////////
                sem_wait(&sem_list_exec_order_right);
                    append_boat(list_exec_order_right, equity_arguments->boat);
                    sem_wait(&sem_schedule_right); schedule(calendarization_algorithm,list_exec_order_right); sem_post(&sem_schedule_right);
                sem_post(&sem_list_exec_order_right);
                //printf("Thread: %d despues de sem_exec_order_right\n", arguments->direction);//todo/////////////////////////////////////////////////////////////////////////////
            }

            pthread_create(&thread_boat, NULL, equity_thread_func, equity_arguments);
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
            printf("Created thread for Boat: %d\n", equity_arguments->boat->id);//todo/////////////////////////////////////////////////////////////////////////////
        }
        else//If empty list
        {
            ///Wait for boat threads
            //
            /*
            Thread_Doubly_Linked_List_Node_t* piv;
            if(arguments->direction==RIGHT)
            {
                piv=thread_list_left->first;
                while(piv!=NULL)
                {
                    pthread_join(piv->data, NULL);
                    piv = piv->next;
                    delete_first_thread(thread_list_left);
                }
            }
            else
            {
                piv=thread_list_right->first;
                while(piv != NULL)
                {
                    pthread_join(piv->data, NULL);
                    piv = piv->next;
                    delete_first_thread(thread_list_right);
                }
            }
            break;//todo Temporal
            */
            //
        }
    }
}
void equity(int w)
{
    ///Print Equity value
    printf("Equity\n");
    printf("W: %d\n", w);

    ///Int for boats count to switch direction semaphores
    int* finished_boats_ptr = malloc(sizeof(int)); *finished_boats_ptr=0;

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
    thread_launcher_arguments_left->finished_boats_ptr=finished_boats_ptr;
    pthread_create(&thread_launcher_left, NULL, equity_thread_launcher_func, thread_launcher_arguments_left);

    ///Creating Launcher thread for right boats
    pthread_t thread_launcher_right;
    Equity_arguments_launcher_t* thread_launcher_arguments_right = malloc(sizeof(Equity_arguments_launcher_t));
    thread_launcher_arguments_right->direction=LEFT;
    thread_launcher_arguments_right->w=w;
    thread_launcher_arguments_right->finished_boats_ptr=finished_boats_ptr;
    pthread_create(&thread_launcher_right, NULL, equity_thread_launcher_func, thread_launcher_arguments_right);

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

            //sem_wait(&(*arguments->sem_change_sign_flag));
            //*(arguments->change_sign_flag)=0;
            //sem_post(&(*arguments->sem_change_sign_flag));
        }
        sem_post(&sem_direction_aux);///Se apropia de la edicion de los semaforos de direccion

        //sem_wait(&sem_schedule_left); schedule(calendarization_algorithm,list_exec_order_left); sem_post(&sem_schedule_left);

        while((list_exec_order_left->first->data->id!=arguments->boat->id) || (*(arguments->change_sign_flag)==1 && boats_remaining_right!=0))///Busy waiting hasta que le toque su turno
        {/*Busy waiting equis de*/}
        //printf("%d antes de sem_left\n", arguments->boat->id);//todo/////////////////////////////////////////////////////////////////////////////
        sem_wait(&sem_left);///Se come un permiso de entrar por la izquierda
        //printf("%d despues de sem_left\n", arguments->boat->id);//todo/////////////////////////////////////////////////////////////////////////////
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

        while((list_exec_order_right->first->data->id!=arguments->boat->id) || (*(arguments->change_sign_flag)==1 && boats_remaining_left!=0))///Busy waiting hasta que le toque su turno
        {/*Busy waiting equis de*/}
        //printf("%d antes de sem_right\n", arguments->boat->id);//todo/////////////////////////////////////////////////////////////////////////////
        sem_wait(&sem_right);///Se come un permiso de entrar por la derecha
        //printf("%d despues de sem_right\n", arguments->boat->id);//todo/////////////////////////////////////////////////////////////////////////////
    }

    ///Recorre el canal
    for(int i=0; i<=canal_length; i++)
    {
        if(arguments->boat->direction==RIGHT)///Si el barco va para la derecha
        {
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
                    ups(&sem_right, canal_length);///Da n permisos a la derecha
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
    while(loop)//todo hacer q otro thread cambie una variable booleana global <continue> que cambie con un getchar()
        //while(1)
    {
        if(!is_boat_list_empty(list))
        {
            ///Creating boat thread
            pthread_t thread_boat;
            Sign_arguments_t* sign_arguments = malloc(sizeof(Sign_arguments_t));
            sign_arguments->change_sign_flag=arguments->change_sign_flag;
            sign_arguments->boat=list->first->data;
            sign_arguments->sem_change_sign_flag=arguments->sem_change_sign_flag;

            //printf("Thread: %d antes de sem_create_order\n", arguments->direction);//todo/////////////////////////////////////////////////////////////////////////////
            sem_wait(&sem_list_creation_order);
                append_boat(list_creation_order, sign_arguments->boat);
            sem_post(&sem_list_creation_order);
            //printf("Thread: %d despues de sem_create_order\n", arguments->direction);//todo/////////////////////////////////////////////////////////////////////////////

            if(sign_arguments->boat->direction==RIGHT)
            {
                //printf("Thread: %d antes de sem_exec_order_left\n", arguments->direction);//todo/////////////////////////////////////////////////////////////////////////////
                sem_wait(&sem_list_exec_order_left);
                    append_boat(list_exec_order_left, sign_arguments->boat);
                sem_wait(&sem_schedule_left); schedule(calendarization_algorithm,list_exec_order_left); sem_post(&sem_schedule_left);
                sem_post(&sem_list_exec_order_left);
                //printf("Thread: %d despues de sem_exec_order_left\n", arguments->direction);//todo/////////////////////////////////////////////////////////////////////////////
            }
            else
            {
                //printf("Thread: %d antes de sem_exec_order_right\n", arguments->direction);//todo/////////////////////////////////////////////////////////////////////////////
                sem_wait(&sem_list_exec_order_right);
                    append_boat(list_exec_order_right, sign_arguments->boat);
                sem_wait(&sem_schedule_right); schedule(calendarization_algorithm,list_exec_order_right); sem_post(&sem_schedule_right);
                sem_post(&sem_list_exec_order_right);
                //printf("Thread: %d despues de sem_exec_order_right\n", arguments->direction);//todo/////////////////////////////////////////////////////////////////////////////
            }

            ///Create thread
            pthread_create(&thread_boat, NULL, sign_thread_func, sign_arguments);

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
            printf("Created thread for Boat: %d\n", sign_arguments->boat->id);//todo/////////////////////////////////////////////////////////////////////////////
        }
        else//If empty list
        {
            ///Wait for boat threads
        }
    }
}
void sign(int signboard_time)
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

    while(loop)
    {
        if((*change_sign_flag)==1)
        {
            ///Busy waiting
        }
        else
        {
            sleep(signboard_time);
            sem_wait(&sem_change_sign_flag);
            (*change_sign_flag)=1;
            printf("Time out\n");
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

            //sem_wait(&(*arguments->sem_change_sign_flag));
            //*(arguments->change_sign_flag)=0;
            //sem_post(&(*arguments->sem_change_sign_flag));
        }
        sem_post(&sem_direction_aux);///Se apropia de la edicion de los semaforos de direccion

        //sem_wait(&sem_schedule_left); schedule(calendarization_algorithm,list_exec_order_left); sem_post(&sem_schedule_left);

        while((list_exec_order_left->first->data->id!=arguments->boat->id))///Busy waiting hasta que le toque su turno
        {/*Busy waiting equis de*/}
        //printf("%d antes de sem_left\n", arguments->boat->id);//todo/////////////////////////////////////////////////////////////////////////////
        sem_wait(&sem_left);///Se come un permiso de entrar por la izquierda
        //printf("%d despues de sem_left\n", arguments->boat->id);//todo/////////////////////////////////////////////////////////////////////////////
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

        while((list_exec_order_right->first->data->id!=arguments->boat->id))///Busy waiting hasta que le toque su turno
        {/*Busy waiting equis de*/}
        //printf("%d antes de sem_right\n", arguments->boat->id);//todo/////////////////////////////////////////////////////////////////////////////
        sem_wait(&sem_right);///Se come un permiso de entrar por la derecha
        //printf("%d despues de sem_right\n", arguments->boat->id);//todo/////////////////////////////////////////////////////////////////////////////
    }

    ///Recorre el canal
    for(int i=0; i<=canal_length; i++)
    {
        if(arguments->boat->direction==RIGHT)///Si el barco va para la derecha
        {
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
                sem_wait(&sem_list_exec_order_left);
                append_boat(list_exec_order_left, tico_arguments->boat);
                sem_wait(&sem_schedule_left); schedule(calendarization_algorithm,list_exec_order_left); sem_post(&sem_schedule_left);
                sem_post(&sem_list_exec_order_left);
                //printf("Thread: %d despues de sem_exec_order_left\n", arguments->direction);//todo/////////////////////////////////////////////////////////////////////////////
            }
            else
            {
                //printf("Thread: %d antes de sem_exec_order_right\n", arguments->direction);//todo/////////////////////////////////////////////////////////////////////////////
                sem_wait(&sem_list_exec_order_right);
                append_boat(list_exec_order_right, tico_arguments->boat);
                sem_wait(&sem_schedule_right); schedule(calendarization_algorithm,list_exec_order_right); sem_post(&sem_schedule_right);
                sem_post(&sem_list_exec_order_right);
                //printf("Thread: %d despues de sem_exec_order_right\n", arguments->direction);//todo/////////////////////////////////////////////////////////////////////////////
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


    ///Waiting for launcher threads
    pthread_join(thread_launcher_left, NULL);
    pthread_join(thread_launcher_right, NULL);
}


int main()
{
    ///Leer valores del file
    int w;
    int signboard_time;
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
    struct json_object *SJF;
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


    ///Crea el canal
    canal_init();


    ///Carga los barcos en las listas
    initial_load(load);


    ///El calendarizador ordena 1 vez inicialmente si las listas tienen carga inicial
    if(!is_boat_list_empty(list_left))
    {
        schedule(calendarization_algorithm, list_left);
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


    switch(canal_algorithm)//todo
    {
        case 0: equity(w); break;
        case 1: sign(signboard_time); break;
        case 2: tico(); break;
        default: printf("No se selecciono un metodo de control de flujo valido\n");
    }


    pthread_join(boat_spawner_thread, NULL);


    ///Prints
    printf("\n");
    printf("Creation order: "); print_list(list_creation_order);
    printf("Execution order: "); print_list(list_exec_order_general);
    print_canal();
    printf("Sem Left: "); print_semaphore(&sem_left);
    printf("Sem right: "); print_semaphore(&sem_right);


    return 0;
}
