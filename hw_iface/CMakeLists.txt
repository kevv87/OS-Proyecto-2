cmake_minimum_required(VERSION 3.10)

# Nombre del proyecto 
project(Hardware_Iface)

# Add subdirectory for common
# Aqui es necesario especificar tambien donde van a estar los binarios para que se construya correctamente
# Es necesario que en esta carpeta tambien haya un CMakeLists
add_subdirectory("${PROJECT_SOURCE_DIR}/../common/" "${PROJECT_BINARY_DIR}/common")

# Setting sources files
set(SOURCES serial_ex.c)

# Adding sources al executable
# Tambien aqui se crea el target, con el mismo nombre del project name
add_executable( ${PROJECT_NAME} ${SOURCES} )

# Aqui linkeamos el target al nombre de una libreria
# Es necesario que la libreria que estamos usando tambien tenga su CMakeLists
target_link_libraries(${PROJECT_NAME} PUBLIC Common m)

# Modificamos el include directory para que tenga al root
target_include_directories(${PROJECT_NAME} PUBLIC "../")

# Esto es para el proceso de linking, aqui conectamos los binarios a la carpeta que contiene los .h
target_include_directories(
	${PROJECT_NAME} PUBLIC
	"${PROJECT_BINARY_DIR}"
	"${PROJECT_SOURCE_DIR}/../common/include"
)
