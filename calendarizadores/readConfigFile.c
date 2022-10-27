// sudo apt install libjson-c-dev

// gcc readConfigFile.c -ljson-c -o readConfigFile
// ./readConfigFile

#include <stdio.h>
#include <json-c/json.h>
#include "types.h"

int main() {
    
    /*
        RR = 0
        Prioridad = 1
        SJF = 2
        FCFS = 3
        EDF = 4
    */
    int calendarization_algorithm;

    /*
        Equidad = 0
        Letrero = 1
        Tico = 2
    */
    int canal_algorithm;

    int canal_length;
    int base_speed;
    int boat_quantity;

    // w = -1 significa q no aplica
    int w;

    //signboard_timer = -1 significa q no aplica
    int signboard_timer;

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
    signboard_timer = json_object_get_int(Signboard);

    load -> left[0] = json_object_get_int(json_object_array_get_idx(DefinedLoadLeft, 0));
    load -> left[1] = json_object_get_int(json_object_array_get_idx(DefinedLoadLeft, 1));
    load -> left[2] = json_object_get_int(json_object_array_get_idx(DefinedLoadLeft, 2));

    load -> right[0] = json_object_get_int(json_object_array_get_idx(DefinedLoadRight, 0));
    load -> right[1] = json_object_get_int(json_object_array_get_idx(DefinedLoadRight, 1));
    load -> right[2] = json_object_get_int(json_object_array_get_idx(DefinedLoadRight, 2));

    printf("calendarization_algorithm = %d\n", calendarization_algorithm);
    printf("canal_algorithm = %d\n", canal_algorithm);
    printf("canal_length = %d\n", canal_length);
    printf("base_speed = %d\n", base_speed);
    printf("boat_quantity = %d\n", boat_quantity);
    printf("w = %d\n", w);
    printf("signboard_timer = %d\n", signboard_timer);

    printf("load left[0] = %d\n", load -> left[0]);
    printf("load left[1] = %d\n", load -> left[1]);
    printf("load left[2] = %d\n", load -> left[2]);

    printf("load right[0] = %d\n", load -> right[0]);
    printf("load right[1] = %d\n", load -> right[1]);
    printf("load right[2] = %d\n", load -> right[2]);

    free(load);
    
    return 0;





















}
