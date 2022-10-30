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










def readConfigFile(file):

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

    file.close()

def gui():

    # set up pygame
    pygame.init()

    # set up the window
    global windowSurface
    windowSurface = pygame.display.set_mode((1209, 680), 0 , 32)

    # set up the window title
    pygame.display.set_caption("Scheduling Ships - Calendarizador de Hilos")

    # set up background
    background = pygame.image.load(BACKGROUND_PATH).convert()

    # set up arrows
    arrowLeft = pygame.image.load(ARROW_LEFT_PATH).convert()
    arrowLeft.set_colorkey([0, 0, 0])

    arrowRight = pygame.image.load(ARROW_RIGHT_PATH).convert()
    arrowRight.set_colorkey([0, 0, 0])

    # set up ships
    ship0 = pygame.image.load(NORMAL_BOAT_LEFT_PATH).convert()
    ship0.set_colorkey([0, 0, 0])

    ship1 = pygame.image.load(NORMAL_BOAT_RIGHT_PATH).convert()
    ship1.set_colorkey([0, 0, 0])

    ship2 = pygame.image.load(FISHING_BOAT_LEFT_PATH).convert()
    ship2.set_colorkey([0, 0, 0])

    ship3 = pygame.image.load(FISHING_BOAT_RIGHT_PATH).convert()
    ship3.set_colorkey([0, 0, 0])

    ship4 = pygame.image.load(PATROL_BOAT_LEFT_PATH).convert()
    ship4.set_colorkey([0, 0, 0])

    ship5 = pygame.image.load(PATROL_BOAT_RIGHT_PATH).convert()
    ship5.set_colorkey([0, 0, 0])

    ship6 = pygame.image.load(NORMAL_BOAT_LEFT_PATH).convert()
    ship6.set_colorkey([0, 0, 0])

    ship7 = pygame.image.load(NORMAL_BOAT_RIGHT_PATH).convert()
    ship7.set_colorkey([0, 0, 0])

    ship8 = pygame.image.load(FISHING_BOAT_LEFT_PATH).convert()
    ship8.set_colorkey([0, 0, 0])

    ship9 = pygame.image.load(FISHING_BOAT_RIGHT_PATH).convert()
    ship9.set_colorkey([0, 0, 0])

    ship10 = pygame.image.load(TRANSPARENT_BOAT_PATH).convert()
    ship10.set_colorkey([0, 0, 0])

    # set up flow control text
    equityText = pygame.image.load(EQUITY_TEXT_PATH).convert()
    equityText.set_colorkey([0, 0, 0])

    signboardText = pygame.image.load(SIGNBOARD_TEXT_PATH).convert()
    signboardText.set_colorkey([0, 0, 0])

    ticoText = pygame.image.load(TICO_TEXT_PATH).convert()
    ticoText.set_colorkey([0, 0, 0])

    # set up the scheduler algorithm text
    rrText = pygame.image.load(RR_TEXT_PATH).convert()
    rrText.set_colorkey([0, 0, 0])

    priorityText = pygame.image.load(PRIORITY_TEXT_PATH).convert()
    priorityText.set_colorkey([0, 0, 0])

    sjfText = pygame.image.load(SJF_TEXT_PATH).convert()
    sjfText.set_colorkey([0, 0, 0])

    fcfsText = pygame.image.load(FCFS_TEXT_PATH).convert()
    fcfsText.set_colorkey([0, 0, 0])

    edfText = pygame.image.load(EDF_TEXT_PATH).convert()
    edfText.set_colorkey([0, 0, 0])





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

    # run the game loop
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
        windowSurface.blit(arrowRight, [85, 100])

        # draw the ships onto the surface
        windowSurface.blit(ship0, [x, 250])
        windowSurface.blit(ship1, [215, 250])
        windowSurface.blit(ship2, [315, 250])
        windowSurface.blit(ship3, [415, 250])
        windowSurface.blit(ship4, [515, 250])
        windowSurface.blit(ship5, [615, 250])
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

        if x <= 1015:
            x += 0.2
        else:
            x = 2000




file = open("calendarizadores/config.json")

readConfigFile(file)

gui()

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