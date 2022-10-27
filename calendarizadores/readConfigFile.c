// sudo apt install libjson-c-dev

// gcc readConfigFile.c -ljson-c -o readConfigFile
// ./readConfigFile

#include <stdio.h>
#include <json-c/json.h>

int main() {
    
    FILE *fp;
    char buffer[1024];

    struct json_object *SchedulerAlgorithm; // calendarization_algorithm
    struct json_object *FlowControlMethod; // canal_algorithmstruct json_object *FlowControlMethod; //
    struct json_object *ChannelLength; // canal_length
    struct json_object *BoatSpeed; // base_speed
    struct json_object *BoatQuantity;
    struct json_object *WParameter; // w
    struct json_object *Signboard; // signboard_time    
    struct json_object *DefinedLoadLeft; // load->left
    struct json_object *DefinedLoadRight; // load->right

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

    printf("SchedulerAlgorithm: %d\n", json_object_get_int(SchedulerAlgorithm));
    printf("FlowControlMethod: %d\n", json_object_get_int(FlowControlMethod));
    printf("ChannelLength: %d\n", json_object_get_int(ChannelLength));
    printf("BoatSpeed: %d\n", json_object_get_int(BoatSpeed));
    printf("BoatQuantity: %d\n", json_object_get_int(BoatQuantity));
    printf("WParameter: %d\n", json_object_get_int(WParameter));
    printf("Signboard: %d\n", json_object_get_int(Signboard));

    loadSize = json_object_array_lenght(DefinedLoadLeft);
    printf("Found %lu defined load\n", loadSize);   

    NormalBoat = json_object_array_get_idx(DefinedLoadLeft, 0);
    FishingBoat = json_object_array_get_idx(DefinedLoadLeft, 1);
    PatrolBoat = json_object_array_get_idx(DefinedLoadLeft, 2);

    printf("NormalBoat: %d\n", json_object_get_int(NormalBoat));
    printf("FishingBoat: %d\n", json_object_get_int(FishingBoat));
    printf("PatrolBoat: %d\n", json_object_get_int(PatrolBoat));
    
    return 0;

}
