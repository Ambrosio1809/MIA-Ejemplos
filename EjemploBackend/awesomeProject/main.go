package main

import (
	"fmt"
	_ "github.com/godror/godror"
	"github.com/rs/cors"
	"log"
	"net/http"
)

func main() {
	fmt.Println("Hola Mundo")
	c := cors.New(cors.Options{
		AllowedOrigins: []string{"*"},
		AllowedMethods: []string{"GET", "POST", "PUT", "DELETE"},
		AllowedHeaders: []string{"*"},
	})
	router := NewRouter()
	fmt.Println("El servidor esta corriendo en http://3.138.114.96:4000")
	log.Fatal(http.ListenAndServe(":5000", c.Handler(router)))
}
