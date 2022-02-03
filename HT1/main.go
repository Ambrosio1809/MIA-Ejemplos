package main

import (
	"bufio"
	"bytes"
	"encoding/binary"
	"fmt"
	"log"
	"os"
	"strings"
	"unsafe"
)

type control struct {
	FirstSpace int64
}

type MyStructure struct {
	Nombre    [10]byte
	Apuntador [5]int64
}

func main() {
	menu()
}

func menu() {
	var option = 0
	for {
		fmt.Println("----------------------------------------")
		fmt.Println("------------Escoja una opción-----------")
		fmt.Println("----------------------------------------")
		fmt.Println("------- 1. Crear archivo binario -------")
		fmt.Println("------ 2.Eliminar archivo binario ------")
		fmt.Println("---------  3. Crear estructura ---------")
		fmt.Println("---------- 4. Leer estructura ----------")
		fmt.Println("------------- 5. Renombrar -------------")
		fmt.Println("------------- 6. Graficar --------------")
		fmt.Println("--------------- 7. Salir ---------------")
		fmt.Scanf("%d\n", &option)
		switch option {
		case 1:
			crearArchivo()
			break
		case 2:
			eliminarArchivo()
			break
		case 3:
			crearEstructura()
			break
		case 4:
			leerEstructuras()
			break
		case 5:
			Renombrar()
			break
		case 6:
			Graficar()
			break
		case 7:
			salir()
			break
		}
	}
}

func Renombrar() {
	lectura := bufio.NewReader(os.Stdin)
	fmt.Println("Ingrese el nombre del Nodo a Renombrar")
	exNodo, _ := lectura.ReadString('\n')
	exNodo = strings.TrimSpace(exNodo)
	fmt.Println("Ingrese el nuevo nombre del nodo")
	newNodo, _ := lectura.ReadString('\n')
	newNodo = strings.TrimSpace(newNodo)
	fmt.Println("EL nommbre e abuscar es: ", exNodo)
	fmt.Println("El nuevo nombre es ", newNodo)

	//Abrimos el archivo.
	file, err := os.OpenFile("misEstructuras.bin", os.O_RDWR, 0777)
	defer file.Close()
	if err != nil {
		log.Fatal(err)
	}

	control := control{}

	nodoOrigen := MyStructure{}

	nodoNuevo := MyStructure{}

	var sizeControl int64 = int64(unsafe.Sizeof(control))
	var sizeEstructura int64 = int64(unsafe.Sizeof(nodoNuevo))

	file.Seek(0, 0)
	dataControl := leerBytes(file, int(sizeControl))
	bufferControl := bytes.NewBuffer(dataControl)
	err = binary.Read(bufferControl, binary.BigEndian, &control)
	if err != nil {
		log.Fatal("binary.Read failed", err)
	}

	var posicionEscribir = sizeControl + control.FirstSpace*sizeEstructura

	var posicionOrigen = buscarEstructura(file, sizeControl, exNodo)

	file.Seek(posicionOrigen, 0)
	dataOrigen := leerBytes(file, int(sizeEstructura))
	bufferOrigen := bytes.NewBuffer(dataOrigen)
	err = binary.Read(bufferOrigen, binary.BigEndian, &nodoOrigen)
	if err != nil {
		log.Fatal("binary.Read failed", err)
	}

	//actualizamos el primer puntero vacio que encontremos del origen
	for i := 0; i < 5; i++ {
		if nodoOrigen.Apuntador[i] == -1 {
			copy(nodoOrigen.Nombre[:], newNodo)
			break
		}
	}

	//lo reescribimos
	file.Seek(posicionOrigen, 0)
	var bufferOrigenW bytes.Buffer
	binary.Write(&bufferOrigenW, binary.BigEndian, &nodoOrigen)
	escribirBytes(file, bufferOrigenW.Bytes())

	//escribimos el nuevo nodo
	file.Seek(posicionEscribir, 0)
	var bufferNuevo bytes.Buffer
	binary.Write(&bufferNuevo, binary.BigEndian, &nodoNuevo)
	escribirBytes(file, bufferNuevo.Bytes())

	//actualizamos el valor del control
	control.FirstSpace = control.FirstSpace + 1
	file.Seek(0, 0)
	var bufferControlW bytes.Buffer
	binary.Write(&bufferControlW, binary.BigEndian, &control)
	escribirBytes(file, bufferControlW.Bytes())

	fmt.Println("renombrado exitosamente!")

}

func Graficar() {
	fmt.Println("Graficar ")
}

func crearArchivo() {
	fmt.Println("-----Crear archivo binario-----")

	//variable para llevar control del tamaño del disco
	var size = 1

	//se procede a crear el archivo
	file, err := os.Create("misEstructuras.bin")
	defer file.Close()
	if err != nil {
		log.Fatal(err)
	}

	//se crea una variable temporal con un cero que nos ayudará a llenar nuestro archivo de ceros lógicos
	var temporal int8 = 0
	s := &temporal
	var binario bytes.Buffer
	binary.Write(&binario, binary.BigEndian, s)

	/*
		se realiza un for para llenar el archivo completamente de ceros
		NOTA: Para esta parte se recomienda tener un buffer con 1024 ceros (ya que 1024 es la medida
		mínima a escribir) para que este ciclo sea más eficiente
	*/
	for i := 0; i < size*1024; i++ {
		escribirBytes(file, binario.Bytes())
	}

	/*
		se escribira un estudiante por default para llevar el control.
		En el proyecto, el que nos ayuda a llevar el control de las
		particiones es el mbr
	*/

	//Creamos el struct de control y el struct de miEstructura
	miControl := control{FirstSpace: 1}

	nodoRaiz := MyStructure{}
	copy(nodoRaiz.Nombre[:], "Ruth")
	//Inicializamos todos los apuntadores de la estructura en -1
	for i := 0; i < 5; i++ {
		nodoRaiz.Apuntador[i] = -1
	}

	//nos posicionamos al inicio del archivo usando la funcion Seek
	file.Seek(0, 0)

	//Escribimos struct de control
	var bufferControl bytes.Buffer
	binary.Write(&bufferControl, binary.BigEndian, &miControl)
	escribirBytes(file, bufferControl.Bytes())

	//movemos el puntero a donde ira nuestra primera estructura
	file.Seek(int64(unsafe.Sizeof(miControl)), 0)

	//Escribimos struct raiz
	var bufferNodo bytes.Buffer
	binary.Write(&bufferNodo, binary.BigEndian, &nodoRaiz)
	escribirBytes(file, bufferNodo.Bytes())
}

func eliminarArchivo() {
	fmt.Println("-----Eliminar archivo binario-----")

	err := os.Remove("misEstructuras.bin")

	if err != nil {
		fmt.Println("Error al eliminar el archivo.")
	} else {
		fmt.Println("Archivo eliminado exitosamente!")
	}
}

func crearEstructura() {
	fmt.Println("-----Crear estudiante-----")

	//Toma los datos de entrada del usuario
	reader := bufio.NewReader(os.Stdin)
	fmt.Print("Ingrese el origen:")
	origen, _ := reader.ReadString('\n')
	origen = strings.TrimSpace(origen)

	fmt.Print("Ingrese el nombre:")
	nombre, _ := reader.ReadString('\n')
	nombre = strings.TrimSpace(nombre)

	//Abrimos el archivo.
	file, err := os.OpenFile("misEstructuras.bin", os.O_RDWR, 0777)
	defer file.Close()
	if err != nil {
		log.Fatal(err)
	}

	//declaramos la variable control que nos ayudara a escribir el nuevo
	control := control{}

	//declaramos una estructura para leer el nodo de origen
	nodoOrigen := MyStructure{}

	//declaramos el nodo nuevo a crear con sus datos
	nodoNuevo := MyStructure{}
	copy(nodoNuevo.Nombre[:], nombre)
	for i := 0; i < 5; i++ {
		nodoNuevo.Apuntador[i] = -1
	}

	//obtenemor el size del control para empezar a leer desde ahi
	var sizeControl int64 = int64(unsafe.Sizeof(control))
	//obtenemos el size de la estructrua que nos servira en otras operaciones
	var sizeEstructura int64 = int64(unsafe.Sizeof(nodoNuevo))

	//leemos el control
	file.Seek(0, 0)
	dataControl := leerBytes(file, int(sizeControl))
	bufferControl := bytes.NewBuffer(dataControl)
	err = binary.Read(bufferControl, binary.BigEndian, &control)
	if err != nil {
		log.Fatal("binary.Read failed", err)
	}

	//obtenemos la posicion donde toca escribir
	var posicionEscribir = sizeControl + control.FirstSpace*sizeEstructura

	//leemos el nodo origen para actualizar el apuntador
	var posicionOrigen = buscarEstructura(file, sizeControl, origen)

	file.Seek(posicionOrigen, 0)
	dataOrigen := leerBytes(file, int(sizeEstructura))
	bufferOrigen := bytes.NewBuffer(dataOrigen)
	err = binary.Read(bufferOrigen, binary.BigEndian, &nodoOrigen)
	if err != nil {
		log.Fatal("binary.Read failed", err)
	}

	//actualizamos el primer puntero vacio que encontremos del origen
	for i := 0; i < 5; i++ {
		if nodoOrigen.Apuntador[i] == -1 {
			nodoOrigen.Apuntador[i] = posicionEscribir
			break
		}
	}

	//lo reescribimos
	file.Seek(posicionOrigen, 0)
	var bufferOrigenW bytes.Buffer
	binary.Write(&bufferOrigenW, binary.BigEndian, &nodoOrigen)
	escribirBytes(file, bufferOrigenW.Bytes())

	//escribimos el nuevo nodo
	file.Seek(posicionEscribir, 0)
	var bufferNuevo bytes.Buffer
	binary.Write(&bufferNuevo, binary.BigEndian, &nodoNuevo)
	escribirBytes(file, bufferNuevo.Bytes())

	//actualizamos el valor del control
	control.FirstSpace = control.FirstSpace + 1
	file.Seek(0, 0)
	var bufferControlW bytes.Buffer
	binary.Write(&bufferControlW, binary.BigEndian, &control)
	escribirBytes(file, bufferControlW.Bytes())

	fmt.Println("Creado exitosamente!")
}

/*
	Metodo que buscara un nombre especifico dentro de la estructura
	(origen representa el nombre que se buscara dentro de la esructura)
	posicionEstructura nos ayuda hacer el arbol recursivo para poder
	recorrer
*/
func buscarEstructura(file *os.File, posicionEstructura int64, origen string) int64 {

	//Se mueve el puntero hacia la nueva posicion
	file.Seek(posicionEstructura, 0)

	//Se declara una estructura temporal para leer la estructura del archivo
	estructuraTemporal := MyStructure{}
	var size int = int(unsafe.Sizeof(estructuraTemporal))

	//Lee la cantidad de <size> bytes del archivo
	data := leerBytes(file, size)

	//Convierte la data en un buffer,necesario para
	//decodificar binario
	buffer := bytes.NewBuffer(data)

	//Decodificamos y guardamos en la variable estudianteTemporal
	err := binary.Read(buffer, binary.BigEndian, &estructuraTemporal)

	if err != nil {
		log.Fatal("binary.Read failed", err)
		return -1

	} else {
		temporal := string(estructuraTemporal.Nombre[:len(origen)])
		if origen == temporal {
			return posicionEstructura

		} else {

			var posicionInicio int64 = -1

			for i := 0; i < 5 && posicionInicio == -1; i++ {
				if estructuraTemporal.Apuntador[i] != -1 {
					posicionInicio = buscarEstructura(file, estructuraTemporal.Apuntador[i], origen)
				}
			}

			return posicionInicio
		}
	}
}

func leerEstructuras() {
	fmt.Println("-----Leer Estructuras-----")

	//Abrimos el archivo.
	file, err := os.Open("misEstructuras.bin")
	defer file.Close()
	if err != nil {
		log.Fatal(err)
	}

	//obtenemor el size del control para empezar a leer desde ahi
	var size int64 = int64(unsafe.Sizeof(control{}))
	fmt.Println(leerEstructura(file, size, 1))
}

/*
	Este metodo será un método recursivo que recibira la posicion en la cual se encuentra
	la estructura que se desea leer, y devolverá el string de la concatenación de nombres
*/
func leerEstructura(file *os.File, posicionEstructura int64, nivel int) string {

	//Se mueve el puntero hacia la nueva posicion
	file.Seek(posicionEstructura, 0)

	//Se declara una estructura temporal para leer la estructura del archivo
	estructuraTemporal := MyStructure{}
	var size int = int(unsafe.Sizeof(estructuraTemporal))

	//Lee la cantidad de <size> bytes del archivo
	data := leerBytes(file, size)

	//Convierte la data en un buffer,necesario para
	//decodificar binario
	buffer := bytes.NewBuffer(data)

	//Decodificamos y guardamos en la variable estudianteTemporal
	err := binary.Read(buffer, binary.BigEndian, &estructuraTemporal)

	if err != nil {
		log.Fatal("binary.Read failed", err)
		return "Error!"

	} else {
		//Si se leyo exitosamente los datos
		//declaramos un string temporal para almacenar los nombres de las demas estructuras
		nombres := string(estructuraTemporal.Nombre[:]) + "\n"

		//hacemos un ciclo para recorrer todos los apuntadores y concatenamos el nombre
		for i := 0; i < 5; i++ {
			if estructuraTemporal.Apuntador[i] != -1 {
				for j := 0; j < nivel; j++ {
					nombres += "     "
				}
				nombres += leerEstructura(file, estructuraTemporal.Apuntador[i], nivel+1)
			}
		}

		return (nombres)
	}

}

func salir() {
	os.Exit(0)
}

func leerBytes(file *os.File, number int) []byte {
	bytes := make([]byte, number)

	_, err := file.Read(bytes)
	if err != nil {
		log.Fatal(err)
	}

	return bytes
}

func escribirBytes(file *os.File, bytes []byte) {
	_, err := file.Write(bytes)

	if err != nil {
		log.Fatal(err)
	}
}
