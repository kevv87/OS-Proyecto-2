"""pygame
    ✓ addShip()
    ✓ changeDirection() 
    interchangeShipsPosition()
    placeShipsInPosition()
    addShipToArray()
    ✓ delShip()

    2 semaforos, 1 izq y otro der
    La direccion del canal la maneja el semaforo que está en 0
"""

# set up libraries
import sys
import json
import pygame
from pygame.locals import *
from threading import *
from models import *
from constants import *

def readConfigFile(file):

    configList = []

    # returns JSON object as a dictionary
    data = json.load(file)

    configList.append(data["SchedulerAlgorithm"])
    configList.append(data["FlowControlMethod"])
    configList.append(data["ChannelLength"])
    configList.append(data["BoatSpeed"])
    configList.append(data["BoatQuantity"])
    configList.append(data["WParameter"])
    configList.append(data["Signboard"])
    configList.append(data["DefinedLoadLeft"])
    configList.append(data["DefinedLoadRight"])
    configList.append(data["Priority"])
    configList.append(data["EDF"])

    file.close()

    return configList

def getSchedulerAlgorithmText(schedulerAlgorithm):

    match schedulerAlgorithm:
        
        case 0:

            return RR_TEXT_PATH

        case 1:

            return PRIORITY_TEXT_PATH
            
        case 2:

            return SJF_TEXT_PATH
            
        case 3:

            return FCFS_TEXT_PATH
            
        case 4:

            return EDF_TEXT_PATH

        case default:

            return RR_TEXT_PATH

def getFlowControlMethodText(flowControl):

    match flowControl:
        
        case 0:

            return EQUITY_TEXT_PATH

        case 1:

            return SIGNBOARD_TEXT_PATH

        case 2:

            return TICO_TEXT_PATH

        case default:

            return EQUITY_TEXT_PATH

def changeArrowDirection():
    
    if(arrowCurrent == arrowList[LEFT]):

        arrowCurrent = arrowList[RIGHT]

    else:

        arrowCurrent = arrowList[LEFT]

    #return arrowCurrent

def updateReadyQueueText(readyQueueNumber):

    font = pygame.font.Font("freesansbold.ttf", 75)

    return font.render(str(readyQueueNumber), True, (255, 255, 255))

def getBoatImagePath(type, direction):

    match type:        

        case 0:

            if(direction == LEFT):

                return NORMAL_BOAT_LEFT_PATH

            else:

                return NORMAL_BOAT_RIGHT_PATH

        case 1:

            if(direction == LEFT):
    
                return FISHING_BOAT_LEFT_PATH

            else:

                return FISHING_BOAT_RIGHT_PATH

        case 2:

            if(direction == LEFT):
    
                return PATROL_BOAT_LEFT_PATH

            else:

                return PATROL_BOAT_RIGHT_PATH

def addBoat(id, type, position, direction, speed):

    path = getBoatImagePath(type, direction)

    component = pygame.image.load(path).convert()
    component.set_colorkey([0, 0, 0])

    if(direction == LEFT):

        newBoat = Boat(id, type, position, direction, speed, X9_POSITION, Y_POSITION, component)

    else:

        newBoat = Boat(id, type, position, direction, speed, X0_POSITION, Y_POSITION, component)

    #boatList.insert(position, newBoat)

    boatList.append(newBoat)

def removeBoat(id):

    for i in range(0, len(boatList)):

        if(boatList[i].getId() == id):
            
            del boatList[i]
    
    """
    path = getBoatImagePath(3, 0)

    component = pygame.image.load(path).convert()
    component.set_colorkey([0, 0, 0])

    boatList[position].setComponent(component)
    """

def getFinalPosX(finalPosition):

    match finalPosition:
        
        case 0:

            return X0_POSITION

        case 1:

            return X1_POSITION

        case 2:

            return X2_POSITION

        case 3:

            return X3_POSITION

        case 4:

            return X4_POSITION

        case 5:

            return X5_POSITION

        case 6:

            return X6_POSITION

        case 7:

            return X7_POSITION

        case 8:

            return X8_POSITION

        case 9:

            return X9_POSITION

def moveBoat(id):

    #boat = boatList[0]

    for i in boatList:
    
        if(i.getId() == id):

            boat = i

            break

    speed = boat.getSpeed()

    if(boat.getDirection() == LEFT):

        finalPosition = boat.getPosition() - 1

        finalPosition = getFinalPosX(finalPosition)

        while(True):

            currentPosX = boat.getPosX()                    

            if(currentPosX > finalPosition):

                boat.setPosX(currentPosX - speed)

            else:

                boat.setPosX(finalPosition)

                break

    else:

        finalPosition = boat.getPosition() + 1

        finalPosition = getFinalPosX(finalPosition)

        while(True):

            currentPosX = boat.getPosX()                    

            if(currentPosX < finalPosition):

                boat.setPosX(currentPosX + speed)

            else:

                boat.setPosX(finalPosition)

                break



















file = open("calendarizadores/config.json")

configList = readConfigFile(file)

#def gui(configList):

# set up pygame
pygame.init()

# set up window
global windowSurface
windowSurface = pygame.display.set_mode((1209, 680), 0 , 32)

# set up window title
pygame.display.set_caption("Scheduling Ships - Calendarizador de Hilos")

# set up background
background = pygame.image.load(BACKGROUND_PATH).convert()

# set up arrows    
arrowLeft = pygame.image.load(ARROW_LEFT_PATH).convert()
arrowLeft.set_colorkey([0, 0, 0])

arrowRight = pygame.image.load(ARROW_RIGHT_PATH).convert()
arrowRight.set_colorkey([0, 0, 0])

arrowList = [arrowLeft, arrowRight]

arrowCurrent = arrowList[0]

# set up boats
boatList = []


for i in range(0, configList[CHANNEL_LENGTH_INDEX]):

    #newBoat = pygame.image.load(NORMAL_BOAT_LEFT_PATH).convert()
    #newBoat.set_colorkey([0, 0, 0])

    #boatList.append(newBoat)

    #addBoat(i, 2, 0, 1, 0.00005)
    #addBoat(i, 2, 9, 1, 0.00005)

    pass



#addBoat(0, 2, 0, 0, 0.00005)
addBoat(0, 2, 9, 1, 0.00005)

# get scheduler algorithm text
schedulerAlgorithmText = getSchedulerAlgorithmText(configList[SCHEDULER_ALGORITHM_INDEX])

schedulerAlgorithm = pygame.image.load(schedulerAlgorithmText).convert()
schedulerAlgorithm.set_colorkey([0, 0, 0])

# set up flow control text
flowControlText = getFlowControlMethodText(configList[SCHEDULER_ALGORITHM_INDEX])

flowControl = pygame.image.load(flowControlText).convert()
flowControl.set_colorkey([0, 0, 0])

# set up font for ready queues
font = pygame.font.Font("freesansbold.ttf", 75)

# set up left ready queue text
readyQueueLeftNumber = 0

for i in configList[DEFINED_LOAD_LEFT_INDEX]:

    readyQueueLeftNumber += i

readyQueueLeftText = font.render(str(readyQueueLeftNumber), True, (255, 255, 255))

# set up right ready queue text
readyQueueRightNumber = 0

for i in configList[DEFINED_LOAD_RIGHT_INDEX]:
    
    readyQueueRightNumber += i

readyQueueRightText = font.render(str(readyQueueRightNumber), True, (255, 255, 255))






x = 115

flag = 0

loopFlag = True

# run game loop
while(loopFlag):

    for event in pygame.event.get():

        if event.type == QUIT:

            loopFlag = False

            pygame.quit()

            sys.exit()

    # draw background onto the surface
    windowSurface.blit(background, [0, 0])

    # draw arrow onto the surface
    windowSurface.blit(arrowCurrent, [20, 20])

    #increment = 0

    # draw boats onto the surface
    #for i in range(0, configList[CHANNEL_LENGTH_INDEX]):
    #for i in range(0, len(boatList)):
    for boat in boatList:

        windowSurface.blit(boat.getComponent(), [boat.getPosX(), boat.getPosY()])

        #increment += 100

    # draw ready queue left text
    windowSurface.blit(readyQueueLeftText, [112, 400])

    # draw ready queue right text
    windowSurface.blit(readyQueueRightText, [1012, 400])

    # draw scheduler algorithm text
    windowSurface.blit(schedulerAlgorithm, [25, 570])

    # draw flow control text
    windowSurface.blit(flowControl, [885, 570])

    # draw window onto the screen>
    pygame.display.update()

    #readyQueueRightNumber -= 1

    #readyQueueRightText = updateReadyQueueText(readyQueueRightNumber)

    #arrowCurrent = changeArrowDirection(arrowCurrent)

    #removeBoat(0)


    
    if x <= 1015:
        x += 0.1

        #boatList[0].setPosX(x)

        

    else:

        if(flag == 0):           

            #removeBoat(flag)
            flag = 1

            # create threads
            thread0 = Thread(target = moveBoat, args = (0,))

            # start threads
            thread0.start()

            






    


    #moveBoat(0)

    































#file = open("calendarizadores/config.json")

#configList = readConfigFile(file)

#gui(configList)
