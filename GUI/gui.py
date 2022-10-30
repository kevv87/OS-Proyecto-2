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



import pygame, sys
from pygame.locals import *

#Set up the colors
BLACK = (0,0,0)
RED = (255,0,0)
GREEN = (0,255,0)
BLUE = (0,0,255)
WHITE = (255,255,255)

# set up pygame
pygame.init()
 
# set up the window
windowSurface = pygame.display.set_mode((1209, 680), 0 , 32)
pygame.display.set_caption("Scheduling Ships - Calendarizador de Hilos")

# set up background
background = pygame.image.load("GUI/images/ocean.png").convert()

# set up arrows
arrowLeft = pygame.image.load("GUI/images/arrows/arrowLeft.png").convert()
arrowLeft.set_colorkey([0, 0, 0])

arrowRight = pygame.image.load("GUI/images/arrows/arrowRight.png").convert()
arrowRight.set_colorkey([0, 0, 0])

# set up ships
ship0 = pygame.image.load("GUI/images/boats/normalBoatLeft.png").convert()
ship0.set_colorkey([0, 0, 0])

ship1 = pygame.image.load("GUI/images/boats/normalBoatRight.png").convert()
ship1.set_colorkey([0, 0, 0])

ship2 = pygame.image.load("GUI/images/boats/fishingBoatLeft.png").convert()
ship2.set_colorkey([0, 0, 0])

ship3 = pygame.image.load("GUI/images/boats/fishingBoatRight.png").convert()
ship3.set_colorkey([0, 0, 0])

ship4 = pygame.image.load("GUI/images/boats/patrolBoatLeft.png").convert()
ship4.set_colorkey([0, 0, 0])

ship5 = pygame.image.load("GUI/images/boats/patrolBoatRight.png").convert()
ship5.set_colorkey([0, 0, 0])

ship6 = pygame.image.load("GUI/images/boats/normalBoatLeft.png").convert()
ship6.set_colorkey([0, 0, 0])

ship7 = pygame.image.load("GUI/images/boats/normalBoatRight.png").convert()
ship7.set_colorkey([0, 0, 0])

ship8 = pygame.image.load("GUI/images/boats/fishingBoatLeft.png").convert()
ship8.set_colorkey([0, 0, 0])

ship9 = pygame.image.load("GUI/images/boats/fishingBoatRight.png").convert()
ship9.set_colorkey([0, 0, 0])

ship10 = pygame.image.load("GUI/images/boats/transparentBoat.png").convert()
ship10.set_colorkey([0, 0, 0])

# set up flow control text
equityText = pygame.image.load("GUI/images/texts/equity.png").convert()
equityText.set_colorkey([0, 0, 0])

signboardText = pygame.image.load("GUI/images/texts/signboard.png").convert()
signboardText.set_colorkey([0, 0, 0])

ticoText = pygame.image.load("GUI/images/texts/tico.png").convert()
ticoText.set_colorkey([0, 0, 0])

# set up the scheduler algorithm text
rrText = pygame.image.load("GUI/images/texts/rr.png").convert()
rrText.set_colorkey([0, 0, 0])

priorityText = pygame.image.load("GUI/images/texts/priority.png").convert()
priorityText.set_colorkey([0, 0, 0])

sjfText = pygame.image.load("GUI/images/texts/sjf.png").convert()
sjfText.set_colorkey([0, 0, 0])

fcfsText = pygame.image.load("GUI/images/texts/fcfs.png").convert()
fcfsText.set_colorkey([0, 0, 0])

edfText = pygame.image.load("GUI/images/texts/edf.png").convert()
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

# run the game loop
while loopFlag:

    for event in pygame.event.get():
        if event.type == QUIT:
            loopFlag = False
            pygame.quit()
            sys.exit()

    mousePos = pygame.mouse.get_pos()
    x = mousePos[0]
    y = mousePos[1]

    # draw the background onto the surface
    windowSurface.blit(background, [0, 0])

    # draw the arrow onto the surface
    windowSurface.blit(arrowRight, [85, 100])

    # draw the ships onto the surface
    windowSurface.blit(ship0, [x, y])
    #windowSurface.blit(ship1, [215, 250])
    #windowSurface.blit(ship2, [315, 250])
    #windowSurface.blit(ship3, [415, 250])
    #windowSurface.blit(ship4, [515, 250])
    #windowSurface.blit(ship10, [615, 250])
    #windowSurface.blit(ship6, [715, 250])
    #windowSurface.blit(ship7, [815, 250])
    #windowSurface.blit(ship8, [915, 250])
    #windowSurface.blit(ship9, [1015, 250])

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