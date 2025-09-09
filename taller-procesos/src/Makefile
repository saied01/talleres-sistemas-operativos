# Nombre del compilador
CC = gcc

# Flags de compilación (warnings y depuración)
CFLAGS = -Wall -Wextra -g

# Nombre del ejecutable
TARGET = juego

# Regla por defecto
all: $(TARGET)

# Cómo generar el ejecutable
$(TARGET): ejercicio1.c
	$(CC) $(CFLAGS) -o $(TARGET) ejercicio1.c

# Regla para limpiar archivos generados
clean:
	rm -f $(TARGET) *.o