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
	fmt.Println("El servidor esta corriendo en http://18.217.252.94:4000")
	log.Fatal(http.ListenAndServe(":4000", c.Handler(router)))
}
