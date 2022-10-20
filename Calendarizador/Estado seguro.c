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



///Globals
int initial_id=0;
int threaded_boats_remaining_left=0;
int threaded_boats_remaining_right=0;

Boat_Doubly_Linked_List_t* creation_order;
Boat_Doubly_Linked_List_t* exec_general_order;
Boat_Doubly_Linked_List_t* exec_order_left;
Boat_Doubly_Linked_List_t* exec_order_right;

Thread_Doubly_Linked_List_t* thread_list_left;
Thread_Doubly_Linked_List_t* thread_list_right;


///Semaphores
sem_t sem_left;//To block left boats
sem_t sem_right;//To block right boats
sem_t sem_direction_aux;//To switch direction
sem_t sem_create_order;//To debug general creation order
sem_t sem_exec_order_left;//To force by busy waiting the left order
sem_t sem_exec_order_right;//To force by busy waiting the left order
sem_t sem_exec_general_order;//To debug total execution order
sem_t sem_thread_list;//To appropriate threads list
sem_t sem_threaded_boats_remaining_aux;//To appropriate threads list



//Defs
void view_resource();
void view_semaphores();
void ups(sem_t* sem, int value);



void view_semaphores()
{
    int sem_left_v;
    sem_getvalue(&sem_left, &sem_left_v);printf("\nSem Left value: %d\n", sem_left_v);
    int sem_right_v;
    sem_getvalue(&sem_right, &sem_right_v);printf("\nSem Left value: %d\n", sem_right_v);
}
int get_id()
{
    return initial_id++;
}
void initial_load(Boat_Doubly_Linked_List_t* list_left, Boat_Doubly_Linked_List_t* list_right, Load_t* load, int base_speed)
{
    while(load->left[0] > 0)
    {
        Boat_t* normal_boat = malloc(sizeof(Boat_t));
        normal_boat->id=get_id(); normal_boat->type=0; normal_boat->position=0; normal_boat->direction=RIGHT; normal_boat->speed=base_speed; normal_boat->life_time=LIFE_TIME_IMMORTAL;
        append_boat(list_left, normal_boat);
        load->left[0]--;
    }
    while(load->left[1] > 0)
    {
        Boat_t* fisher_boat = malloc(sizeof(Boat_t));
        fisher_boat->id=get_id(); fisher_boat->type=1; fisher_boat->position=0; fisher_boat->direction=RIGHT; fisher_boat->speed=base_speed*FISHER_BOAT_SPEED_MULTIPLIER;fisher_boat->life_time=LIFE_TIME_IMMORTAL;
        append_boat(list_left, fisher_boat);
        load->left[1]--;
    }
    while(load->left[2] > 0)
    {
        Boat_t* patrol_boat = malloc(sizeof(Boat_t));
        patrol_boat->id=get_id(); patrol_boat->type=2; patrol_boat->position=0; patrol_boat->direction=RIGHT; patrol_boat->speed=base_speed*PATROL_BOAT_SPEED_MULTIPLIER; patrol_boat->life_time=LIFE_TIME_STANDARD;
        append_boat(list_left, patrol_boat);
        load->left[2]--;
    }

    while(load->right[0] > 0)
    {
        Boat_t* normal_boat = malloc(sizeof(Boat_t));
        normal_boat->id=get_id(); normal_boat->type=0; normal_boat->position=0; normal_boat->direction=LEFT; normal_boat->speed=base_speed; normal_boat->life_time=LIFE_TIME_IMMORTAL;
        append_boat(list_right, normal_boat);
        load->right[0]--;
    }
    while(load->right[1] > 0)
    {
        Boat_t* fisher_boat = malloc(sizeof(Boat_t));
        fisher_boat->id=get_id(); fisher_boat->type=1; fisher_boat->position=0; fisher_boat->direction=LEFT; fisher_boat->speed=base_speed*FISHER_BOAT_SPEED_MULTIPLIER;fisher_boat->life_time=LIFE_TIME_IMMORTAL;
        append_boat(list_right, fisher_boat);
        load->right[1]--;
    }
    while(load->right[2] > 0)
    {
        Boat_t* patrol_boat = malloc(sizeof(Boat_t));
        patrol_boat->id=get_id(); patrol_boat->type=2; patrol_boat->position=0; patrol_boat->direction=LEFT; patrol_boat->speed=base_speed*PATROL_BOAT_SPEED_MULTIPLIER; patrol_boat->life_time=LIFE_TIME_STANDARD;
        append_boat(list_right, patrol_boat);
        load->right[2]--;
    }

}
void canal_init(Canal_node_t* canal, int canal_lenght)
{
    for(int i=0; i<canal_lenght; i++)
    {
        sem_init(&(canal[i].sem), 0, 1);
        canal[i].boat=NULL;
    }
}
void print_canal(Canal_node_t* canal, int canal_lenght)
{
    int value;
    for(int i=0; i<canal_lenght; i++)
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
void print_semaphore(sem_t sem)
{
    int value;
    sem_getvalue(&sem, &value);
    printf(": %d\n", value);
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


void* equity_thread_func(void* arguments_)
{
    ///adquiere los parametros
    Equity_arguments_t* arguments = arguments_;

    ///Direction semaphores
    if(arguments->boat->direction==RIGHT)
    {
        while(exec_order_left->first->data->id!=arguments->boat->id)///Busy waiting hasta que le toque su turno
        {/*Busy waiting equis de*/}
        //printf("%d antes de sem_left\n", arguments->boat->id);//todo/////////////////////////////////////////////////////////////////////////////
        sem_wait(&sem_left);///Se come un permiso de entrar por la izquierda
        //printf("%d despues de sem_left\n", arguments->boat->id);//todo/////////////////////////////////////////////////////////////////////////////
    }
    else
    {
        while(exec_order_right->first->data->id!=arguments->boat->id)///Busy waiting hasta que le toque su turno
        {/*Busy waiting equis de*/}
        //printf("%d antes de sem_right\n", arguments->boat->id);//todo/////////////////////////////////////////////////////////////////////////////
        sem_wait(&sem_right);///Se come un permiso de entrar por la derecha
        //printf("%d despues de sem_right\n", arguments->boat->id);//todo/////////////////////////////////////////////////////////////////////////////
    }

    ///Recorre el canal
    for(int i=0; i<=arguments->canal_length; i++)
    {
        if(arguments->boat->direction==RIGHT)///Si el barco va para la derecha
        {
            arguments->boat->position=i;///Actualiza la posicion del barco
            if(i==arguments->canal_length)///Final
            {
                if((arguments->canal+i-1)->boat->id == arguments->boat->id)///Si el barco de la posicion anterior es si mismo
                {
                    (arguments->canal+i-1)->boat=NULL;///Se borra
                }
                sem_post(&((arguments->canal+i-1)->sem));///Libera el espacio anterior
            }
            else if(i==0)///Inicio
            {
                while(exec_order_left->first->data->id!=arguments->boat->id)///Busy waiting hasta que le toque su turno
                {/*Busy waiting equis de*/}
                //printf("%d antes de sem: %d\n", arguments->boat->id, i);//todo/////////////////////////////////////////////////////////////////////////////
                sem_wait(&((arguments->canal+i)->sem));///Reserva el espacio siguiente
                //printf("%d despues de sem: %d\n", arguments->boat->id, i);//todo/////////////////////////////////////////////////////////////////////////////

                //printf("Entra: %d\n", arguments->boat->id);

                //printf("%d antes de sem_exec_order_left\n", arguments->boat->id);//todo/////////////////////////////////////////////////////////////////////////////
                sem_wait(&sem_exec_order_left);
                    delete_first(exec_order_left);
                sem_post(&sem_exec_order_left);
                //printf("%d despues de sem_exec_order_left\n", arguments->boat->id);//todo/////////////////////////////////////////////////////////////////////////////

                sem_wait(&sem_exec_general_order); append_boat(exec_general_order, arguments->boat); sem_post(&sem_exec_general_order);

                (arguments->canal+i)->boat=arguments->boat;///Pone el bote en esa posicion
            }
            else///En el medio
            {
                sem_wait(&((arguments->canal+i)->sem));///Reserva el espacio siguiente
                if((arguments->canal+i-1)->boat->id == arguments->boat->id)///Si el barco de la posicion anterior es si mismo
                {
                    (arguments->canal+i-1)->boat=NULL;///Se borra
                }
                (arguments->canal+i)->boat=arguments->boat;///Pone el bote en la posicion
                sem_post(&((arguments->canal+i-1)->sem));///Libera el espacio anterior
            }
        }
        else///Si el barco va para la izquierda
        {
            int i_upside_down=(arguments->canal_length-i-1);///i de derecha a izquierda (i al reves)
            arguments->boat->position=i_upside_down;///Actualiza la posicion del barco

            if(i==arguments->canal_length)///Final
            {
                if((arguments->canal+i_upside_down+1)->boat->id == arguments->boat->id)///Si el barco de la posicion anterior es si mismo
                {
                    (arguments->canal+i_upside_down+1)->boat=NULL;///Se borra
                }
                sem_post(&((arguments->canal+i_upside_down+1)->sem));///Libera el espacio anterior
            }
            else if(i==0)///Inicio
            {
                while(exec_order_right->first->data->id!=arguments->boat->id)///Busy waiting hasta que le toque su turno
                {/*Busy waiting equis de*/}
                //printf("%d antes de sem: %d\n", arguments->boat->id, i_upside_down);//todo/////////////////////////////////////////////////////////////////////////////
                sem_wait(&((arguments->canal+i_upside_down)->sem));///Reserva el espacio siguiente
                //printf("%d despues de sem: %d\n", arguments->boat->id, i_upside_down);//todo/////////////////////////////////////////////////////////////////////////////

                //printf("%d antes de sem_exec_order_right\n", arguments->boat->id);//todo/////////////////////////////////////////////////////////////////////////////
                sem_wait(&sem_exec_order_right);
                    delete_first(exec_order_right);
                sem_post(&sem_exec_order_right);
                //printf("%d despues de sem_exec_order_right\n", arguments->boat->id);//todo/////////////////////////////////////////////////////////////////////////////

                sem_wait(&sem_exec_general_order); append_boat(exec_general_order, arguments->boat); sem_post(&sem_exec_general_order);

                (arguments->canal+i_upside_down)->boat=arguments->boat;///Pone el bote en esa posicion
            }
            else///En el medio
            {
                sem_wait(&((arguments->canal+i_upside_down)->sem));///Reserva el espacio siguiente
                if((arguments->canal+i_upside_down+1)->boat->id == arguments->boat->id)///Si el barco de la posicion anterior es si mismo
                {
                    (arguments->canal+i_upside_down+1)->boat=NULL;///Se borra
                }
                (arguments->canal+i_upside_down)->boat=arguments->boat;///Pone el bote en la posicion
                sem_post(&((arguments->canal+i_upside_down+1)->sem));///Libera el espacio anterior
            }
        }
        //printf("step\n");//todo/////////////////////////////////////////////////////////////////////////////
        //printf("\t\t\t\t\t %d\n", (int)((1000000 / arguments->boat->speed) * TIME_FIX_FACTOR));
        usleep((int)((1000000 / arguments->boat->speed) * TIME_FIX_FACTOR));///Simula el tiempo que le toma moverse un espacio
    }
    //printf("\n");//todo/////////////////////////////////////////////////////////////////////////////

    sem_wait(&sem_threaded_boats_remaining_aux);
    sem_wait(&sem_direction_aux);///Se apropia de la edicion de los semaforos de direccion
    if(arguments->boat->direction==RIGHT)///Si el barco va para derecha
    {
        threaded_boats_remaining_left--;///Baja el numero total contemporaneo de barcos que quedan por pasar desde la izquierda
    }
    else///Si el barco va para izquierda
    {
        threaded_boats_remaining_right--;///Baja el numero total contemporaneo de barcos que quedan por pasar desde la derecha
    }

    if(arguments->boat->direction==RIGHT)///Si el barco va para la derecha
    {
        if(*(arguments->finished_boats)==arguments->w-1 || threaded_boats_remaining_left==0)///Si ya pasaron los w barcos o no hay mas barcos que pasar desde la izquierda
        {
            if(threaded_boats_remaining_right==0)///Si no hay mas barcos que pasar desde la derecha
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
        if(*(arguments->finished_boats)==arguments->w-1 || threaded_boats_remaining_right==0)
        {
            if(threaded_boats_remaining_left==0)
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
    sem_post(&sem_threaded_boats_remaining_aux);
}
void* equity_thread_launcher_func(void* arguments_)
{
    Equity_arguments_launcher_t* arguments=arguments_;
    while(1)//todo hacer q otro thread cambie una variable booleana global <continue> que cambie con un getchar()
    {
        if(arguments->list->first!=NULL)
        {
            ///Creating boat thread
            pthread_t thread_boat;
            Equity_arguments_t* equity_arguments = malloc(sizeof(Equity_arguments_t));
            equity_arguments->w=arguments->w;
            equity_arguments->finished_boats=arguments->finished_boats_ptr;
            equity_arguments->canal=arguments->canal;
            equity_arguments->canal_length=arguments->canal_length;
            equity_arguments->boat=arguments->list->first->data;

            sem_wait(&sem_create_order);
                append_boat(creation_order, equity_arguments->boat);
            sem_post(&sem_create_order);

            if(equity_arguments->boat->direction==RIGHT)
            {
                sem_wait(&sem_exec_order_left);
                    append_boat(exec_order_left, equity_arguments->boat);
                sem_post(&sem_exec_order_left);
            }
            else
            {
                sem_wait(&sem_exec_order_right);
                    append_boat(exec_order_right, equity_arguments->boat);
                sem_post(&sem_exec_order_right);
            }

            pthread_create(&thread_boat, NULL, equity_thread_func, equity_arguments);

            if(arguments->direction==RIGHT)
            {
                append_thread(thread_list_left, thread_boat);
            }
            else
            {
                append_thread(thread_list_right, thread_boat);
            }
            printf("Created thread for Boat: %d\n", equity_arguments->boat->id);//todo Hacer q si hay menos de w barcos de un lado no c cague
            delete_first(arguments->list);//todo no borrar nodos para poder ordenar la lista con el calendarizacion luego
        }
        else//If empty list
        {
            ///Wait for boat threads
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
        }
    }
}
void equity(Boat_Doubly_Linked_List_t* list_left, Boat_Doubly_Linked_List_t* list_right, Canal_node_t canal[], int canal_length, int w)
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
    printf("Sem Left: "); print_semaphore(sem_left);
    printf("Sem right: "); print_semaphore(sem_right);

    threaded_boats_remaining_left=get_length(list_left);
    threaded_boats_remaining_right=get_length(list_right);

    ///Creating Launcher thread for left boats
    pthread_t thread_launcher_left;
    Equity_arguments_launcher_t* thread_launcher_arguments_left = malloc(sizeof(Equity_arguments_launcher_t));
    thread_launcher_arguments_left->direction=RIGHT;
    thread_launcher_arguments_left->list=list_left;
    thread_launcher_arguments_left->canal=canal;
    thread_launcher_arguments_left->canal_length=canal_length;
    thread_launcher_arguments_left->w=w;
    thread_launcher_arguments_left->finished_boats_ptr=finished_boats_ptr;
    pthread_create(&thread_launcher_left, NULL, equity_thread_launcher_func, thread_launcher_arguments_left);

    ///Creating Launcher thread for right boats
    pthread_t thread_launcher_right;
    Equity_arguments_launcher_t* thread_launcher_arguments_right = malloc(sizeof(Equity_arguments_launcher_t));
    thread_launcher_arguments_right->direction=LEFT;
    thread_launcher_arguments_right->list=list_right;
    thread_launcher_arguments_right->canal=canal;
    thread_launcher_arguments_right->canal_length=canal_length;
    thread_launcher_arguments_right->w=w;
    thread_launcher_arguments_right->finished_boats_ptr=finished_boats_ptr;
    pthread_create(&thread_launcher_right, NULL, equity_thread_launcher_func, thread_launcher_arguments_right);

    ///Waiting for launcher threads
    pthread_join(thread_launcher_left, NULL);
    pthread_join(thread_launcher_right, NULL);


    ///Prints
    printf("\n");
    printf("Creation order: "); print_list(creation_order);
    printf("Execution order: "); print_list(exec_general_order);
    print_canal(canal, canal_length);
    printf("Sem Left: "); print_semaphore(sem_left);
    printf("Sem right: "); print_semaphore(sem_right);

}
void sign(Boat_Doubly_Linked_List_t* list_left, Boat_Doubly_Linked_List_t* list_right, Canal_node_t* canal, int canal_lenght, int signboard_timer)
{
    printf("Sign\n");
}
void tico(Boat_Doubly_Linked_List_t* list_left, Boat_Doubly_Linked_List_t* list_right, Canal_node_t* canal, int canal_lenght)
{
    printf("Tico\n");
}


int main()
{
    ///Leer valores del file
    int calendarization_algorithm = 2;//SJF
    int canal_algorithm = 0;//Equity
    int canal_length = 8;
    int base_speed = 20;
    int w = 2;//-1 significa q no aplica 1 5 7
    int signboard_timer = -1;//-1 significa q no aplica
    Load_t* load = malloc(sizeof(Load_t));
    load->left[0] = 1;
    load->left[1] = 1;
    load->left[2] = 1;
    load->right[0] = 1;
    load->right[1] = 1;
    load->right[2] = 1;

    ///Crea las estructuras de control
    //thread_list_left = create_thread_list(thread_list_left);
    //thread_list_right = create_thread_list(thread_list_right);
    Boat_Doubly_Linked_List_t* list_left = create_boat_list(list_left);
    Boat_Doubly_Linked_List_t* list_right = create_boat_list(list_right);
    sem_init(&sem_direction_aux, 0, 1);
    sem_init(&sem_create_order, 0, 1);
    sem_init(&sem_exec_order_left, 0, 1);
    sem_init(&sem_exec_order_right, 0, 1);
    sem_init(&sem_exec_general_order, 0, 1);
    sem_init(&sem_thread_list, 0, 1);
    sem_init(&sem_threaded_boats_remaining_aux, 0, 1);
    creation_order = create_boat_list(creation_order);
    exec_order_left = create_boat_list(exec_order_left);
    exec_order_right = create_boat_list(exec_order_right);
    exec_general_order = create_boat_list(exec_general_order);
    thread_list_left=create_thread_list(thread_list_left);
    thread_list_right=create_thread_list(thread_list_right);

    ///Crea el canal
    Canal_node_t canal[canal_length]; canal_init(canal, canal_length);

    ///Carga los barcos en las listas
    initial_load(list_left, list_right, load, base_speed);

    ///El calendarizador ordena 1 vez inicialmente si las listas tienen carga inicial
    if(!is_boat_list_empty(list_left))
    {
        schedule(calendarization_algorithm, list_left);
    }
    if(!is_boat_list_empty(list_right))
    {
        schedule(calendarization_algorithm, list_right);
    }
    switch(canal_algorithm)//todo
    {
        case 0: equity(list_left, list_right, canal, canal_length, w); break;
        case 1: sign(list_left, list_right, canal, canal_length, signboard_timer); break;
        case 2: tico(list_left, list_right, canal, canal_length); break;
        default: printf("No se selecciono un metodo de control de flujo valido\n");
    }
    return 0;
}
//Sacar copia de la casa de Chis y Mari
