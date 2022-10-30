"""
    addShip()
    changeDirection()
    interchangeShipsPosition()
    placeShipsInPosition()
    addShipToArray()
    delShip()

    2 semaforos, 1 izq y otro der
    La direccion del canal la maneja el semaforo que está en 0
"""

"""
# Set up the colors
BLACK = (0,0,0)
RED = (255,0,0)
GREEN = (0,255,0)
BLUE = (0,0,255)
WHITE = (255,255,255)
"""

# set up libraries
import pygame
from pygame.locals import *
import sys
import json

# define constants paths
BACKGROUND_PATH = "GUI/images/ocean.png"

ARROW_LEFT_PATH = "GUI/images/arrows/arrowLeft.png"
ARROW_RIGHT_PATH = "GUI/images/arrows/arrowRight.png"

NORMAL_BOAT_LEFT_PATH = "GUI/images/boats/normalBoatLeft.png"
NORMAL_BOAT_RIGHT_PATH = "GUI/images/boats/normalBoatRight.png"

FISHING_BOAT_LEFT_PATH = "GUI/images/boats/fishingBoatLeft.png"
FISHING_BOAT_RIGHT_PATH = "GUI/images/boats/fishingBoatRight.png"

PATROL_BOAT_LEFT_PATH = "GUI/images/boats/patrolBoatLeft.png"
PATROL_BOAT_RIGHT_PATH = "GUI/images/boats/patrolBoatRight.png"

TRANSPARENT_BOAT_PATH = "GUI/images/boats/transparentBoat.png"

EQUITY_TEXT_PATH = "GUI/images/texts/equity.png"
SIGNBOARD_TEXT_PATH = "GUI/images/texts/signboard.png"
TICO_TEXT_PATH = "GUI/images/texts/tico.png"

RR_TEXT_PATH = "GUI/images/texts/rr.png"
PRIORITY_TEXT_PATH = "GUI/images/texts/priority.png"
SJF_TEXT_PATH = "GUI/images/texts/sjf.png"
FCFS_TEXT_PATH = "GUI/images/texts/fcfs.png"
EDF_TEXT_PATH = "GUI/images/texts/edf.png"

# define constants directions
LEFT = 0
RIGHT = 1

# define constants indexes
SCHEDULER_ALGORITHM_INDEX = 0
FLOW_CONTROL_METHOD_INDEX = 1
CHANNEL_LENGTH_INDEX = 2
BOAT_SPEED_INDEX = 3
BOAT_SPEED_INDEX = 4
W_PARAMETER_INDEX = 5
SIGNBOARD_INDEX = 6
DEFINED_LOAD_LEFT_INDEX = 7
DEFINED_LOAD_RIGHT_INDEX = 8
PRIORITY_INDEX = 9
EDF_INDEX = 10








def readConfigFile(file):

    configList = []

    # returns JSON object as a dictionary
    data = json.load(file)

    """
    RR = 0
    Prioridad = 1
    SJF = 2
    FCFS = 3
    EDF = 4
    """
    global schedulerAlgorithm

    """
    Equidad = 0
    Letrero = 1
    Tico = 2
    """
    global flowControlMethod

    global channelLength
    global boatSpeed
    global boatQuantity
    global wParameter
    global signboard
    global definedLoadLeft
    global definedLoadRight
    global priority
    global edf

    """
    schedulerAlgorithm = data["SchedulerAlgorithm"]
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
    """

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

def changeArrowDirection(arrowCurrent, arrowList):
    
    if(arrowCurrent == arrowList[LEFT]):

        arrowCurrent = arrowList[RIGHT]

    else:

        arrowCurrent = arrowList[LEFT]

    return arrowCurrent





















def gui(configList):

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

    """
    # set up ships
    boat0 = pygame.image.load(NORMAL_BOAT_LEFT_PATH).convert()
    boat0.set_colorkey([0, 0, 0])

    boat1 = pygame.image.load(NORMAL_BOAT_RIGHT_PATH).convert()
    boat1.set_colorkey([0, 0, 0])

    boat2 = pygame.image.load(FISHING_BOAT_LEFT_PATH).convert()
    boat2.set_colorkey([0, 0, 0])

    boat3 = pygame.image.load(FISHING_BOAT_RIGHT_PATH).convert()
    boat3.set_colorkey([0, 0, 0])

    boat4 = pygame.image.load(PATROL_BOAT_LEFT_PATH).convert()
    boat4.set_colorkey([0, 0, 0])

    boat5 = pygame.image.load(PATROL_BOAT_RIGHT_PATH).convert()
    boat5.set_colorkey([0, 0, 0])

    boat6 = pygame.image.load(NORMAL_BOAT_LEFT_PATH).convert()
    boat6.set_colorkey([0, 0, 0])

    boat7 = pygame.image.load(NORMAL_BOAT_RIGHT_PATH).convert()
    boat7.set_colorkey([0, 0, 0])

    boat8 = pygame.image.load(FISHING_BOAT_LEFT_PATH).convert()
    boat8.set_colorkey([0, 0, 0])

    boat9 = pygame.image.load(FISHING_BOAT_RIGHT_PATH).convert()
    boat9.set_colorkey([0, 0, 0])

    boat10 = pygame.image.load(TRANSPARENT_BOAT_PATH).convert()
    boat10.set_colorkey([0, 0, 0])
    """

    # set up boats
    boatList = []

    for i in range(0, configList[CHANNEL_LENGTH_INDEX]):

        newBoat = pygame.image.load(NORMAL_BOAT_LEFT_PATH).convert()
        newBoat.set_colorkey([0, 0, 0])

        boatList.append(newBoat)

    """
    # draw scheduler algorithm text
    #windowSurface.blit(rrText, [25, 570])
    #windowSurface.blit(priorityText, [25, 570])
    #windowSurface.blit(sjfText, [25, 570])
    #windowSurface.blit(fcfsText, [25, 570])
    #windowSurface.blit(edfText, [25, 570])
    """

    # get scheduler algorithm text
    schedulerAlgorithmText = getSchedulerAlgorithmText(configList[SCHEDULER_ALGORITHM_INDEX])

    schedulerAlgorithm = pygame.image.load(schedulerAlgorithmText).convert()
    schedulerAlgorithm.set_colorkey([0, 0, 0])

    """
    # set up flow control text
    equityText = pygame.image.load(EQUITY_TEXT_PATH).convert()
    equityText.set_colorkey([0, 0, 0])

    signboardText = pygame.image.load(SIGNBOARD_TEXT_PATH).convert()
    signboardText.set_colorkey([0, 0, 0])

    ticoText = pygame.image.load(TICO_TEXT_PATH).convert()
    ticoText.set_colorkey([0, 0, 0])
    """

    # set up flow control text
    flowControlText = getFlowControlMethodText(configList[SCHEDULER_ALGORITHM_INDEX])

    flowControl = pygame.image.load(flowControlText).convert()
    flowControl.set_colorkey([0, 0, 0])

    """
    # draw the background onto the surface
    windowSurface.blit(background, [0, 0])

    # draw the arrow onto the surface
    windowSurface.blit(arrowRight, [85, 100])

    # draw the ships onto the surface
    windowSurface.blit(ship0, [115, 250])
    windowSurface.blit(ship1, [215, 250])
    windowSurface.blit(ship2, [315, 250])
    windowSurface.blit(ship3, [415, 250])
    windowSurface.blit(ship4, [515, 250])
    windowSurface.blit(ship10, [615, 250])
    windowSurface.blit(ship6, [715, 250])
    windowSurface.blit(ship7, [815, 250])
    windowSurface.blit(ship8, [915, 250])
    windowSurface.blit(ship9, [1015, 250])

    # draw the scheduler algorithm text
    #windowSurface.blit(rrText, [25, 570])
    windowSurface.blit(priorityText, [25, 570])
    #windowSurface.blit(sjfText, [25, 570])
    #windowSurface.blit(fcfsText, [25, 570])
    #windowSurface.blit(edfText, [25, 570])

    # draw the flow control text
    #windowSurface.blit(equityText, [885, 570])
    windowSurface.blit(signboardText, [885, 570])
    #windowSurface.blit(ticoText, [885, 570])

    # draw the window onto the screen>
    pygame.display.update()
    """

    loopFlag = True

    x = 0

    # run game loop
    while loopFlag:

        for event in pygame.event.get():
            if event.type == QUIT:
                loopFlag = False
                pygame.quit()
                sys.exit()

        #mousePos = pygame.mouse.get_pos()
        #x = mousePos[0]
        #y = mousePos[1]

        # draw the background onto the surface
        windowSurface.blit(background, [0, 0])

        # draw the arrow onto the surface
        windowSurface.blit(arrowCurrent, [20, 20])
        
        """
        windowSurface.blit(boat0, [x, 250])
        windowSurface.blit(boat1, [215, 250])
        windowSurface.blit(boat2, [315, 250])
        windowSurface.blit(boat3, [415, 250])
        windowSurface.blit(boat4, [515, 250])
        windowSurface.blit(boat5, [615, 250])
        windowSurface.blit(boat6, [715, 250])
        windowSurface.blit(boat7, [815, 250])
        windowSurface.blit(boat8, [915, 250])
        windowSurface.blit(boat9, [1015, 250])
        """

        # draw the ships onto the surface
        windowSurface.blit(boatList[0], [x, 250])
        windowSurface.blit(boatList[1], [215, 250])
        windowSurface.blit(boatList[2], [315, 250])
        windowSurface.blit(boatList[3], [415, 250])
        windowSurface.blit(boatList[4], [515, 250])
        windowSurface.blit(boatList[5], [615, 250])
        windowSurface.blit(boatList[6], [715, 250])
        windowSurface.blit(boatList[7], [815, 250])
        windowSurface.blit(boatList[8], [915, 250])
        windowSurface.blit(boatList[9], [1015, 250])
        
        # draw the scheduler algorithm text
        windowSurface.blit(schedulerAlgorithm, [25, 570])

        # draw the flow control text
        windowSurface.blit(flowControl, [885, 570])

        # draw the window onto the screen>
        pygame.display.update()




        #arrowCurrent = changeArrowDirection(arrowCurrent, arrowList)






        if x <= 1015:
            x += 0.1
        else:
            x = 2000































file = open("calendarizadores/config.json")

"""
0 => schedulerAlgorithm
1 => flowControlMethod
2 => channelLength
3 => boatSpeed
4 => boatQuantity
5 => wParameter
6 => signboard
7 => definedLoadLeft
8 => definedLoadRight
9 => priority
10 => edf
"""
configList = readConfigFile(file)

gui(configList)

"""
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
"""