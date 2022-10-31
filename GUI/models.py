class Boat: 
    
    def __init__(self, id, type, position, direction, speed, posX, posY, component):
        self.id = id
        self.type = type
        self.position = position
        self.direction = direction
        self.speed = speed
        self.posX = posX
        self.posY = posY
        self.component = component
    
    def getId(self):
        return self.id

    def setId(self, id):
        self.id = id

    def getType(self):
        return self.type

    def setType(self, type):
        self.type = type

    def getPosition(self):
        return self.position

    def setPosition(self, position):
        self.position = position

    def getDirection(self):
        return self.direction

    def setDirection(self, direction):
        self.direction = direction

    def getSpeed(self):
        return self.speed

    def setSpeed(self, speed):
        self.speed = speed

    def getPosX(self):
        return self.posX

    def setPosX(self, posX):
        self.posX = posX

    def getPosY(self):
        return self.posY

    def setPosY(self, posY):
        self.posY= posY    

    def getComponent(self):
        return self.component

    def setComponent(self, component):
        self.component = component

