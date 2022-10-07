package main

import (
	"encoding/json"
	"fmt"
	"io/ioutil"
	"net/http"
)

func getHola(w http.ResponseWriter, r *http.Request) {
	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode("HOLA MUNDO")
}

type inicioS struct {
	Username string `json:"Username"`
	Password string `json:"Password"`
}

var lgn = logn{}

type logn []inicioS

func Login(w http.ResponseWriter, r *http.Request) {
	var lgn = logn{}
	var inicio inicioS
	reqBody, err := ioutil.ReadAll(r.Body)
	if err != nil {
		fmt.Fprintf(w, "Error datos no validos")
	}
	json.Unmarshal(reqBody, &inicio) //La función Unmarshal() en la codificación del paquete / json se utiliza para descomprimir o decodificar los datos de JSON a la estructura
	lgn = append(lgn, inicio)
	fmt.Println(inicio)
	fmt.Println("USUARIO: ", inicio.Username)
	fmt.Println("PASSWORD: ", inicio.Password)
	w.Header().Set("Content-type", "application/json")
	json.NewEncoder(w).Encode(inicio.Username)

}

type Archivo struct {
	Contenido string `json:"Username"`
}

func Carga(w http.ResponseWriter, r *http.Request) {
	reqBody, err := ioutil.ReadAll(r.Body)
	if err != nil {
		fmt.Fprintf(w, "Error datos no validos")
	}
	content := string(reqBody)
	fmt.Println(content)
	w.Header().Set("Content-type", "application/json")
	json.NewEncoder(w).Encode("nice")
}
