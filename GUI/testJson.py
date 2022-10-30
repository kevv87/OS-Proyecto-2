import json

f = open("calendarizadores/config.json")

# returns JSON object as a dictionary
data = json.load(f)

"""
    RR = 0
    Prioridad = 1
    SJF = 2
    FCFS = 3
    EDF = 4
"""
schedulerAlgorithm = data["SchedulerAlgorithm"]

"""
    Equidad = 0
    Letrero = 1
    Tico = 2
"""
flowControlMethod = data["FlowControlMethod"]

channelLength = data["ChannelLength"]
boatSpeed = data["BoatSpeed"]
boatQuantity = data["BoatQuantity"]
wParameter = data["WParameter"]
signboard = data["Signboard"]
definedLoadLeft = data["DefinedLoadLeft"]
definedLoadRight = data["DefinedLoadRight"]
priority = data["Priority"]
edf = data["EDF"]

f.close()


print(schedulerAlgorithm)
print(flowControlMethod)
print(channelLength)
print(boatSpeed)
print(boatQuantity)
print(wParameter)
print(signboard)
print(definedLoadLeft)
print(definedLoadRight)
print(priority)
print(edf)
