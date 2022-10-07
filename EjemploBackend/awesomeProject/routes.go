package main

import (
	"github.com/gorilla/mux"
	"net/http"
)

type Route struct {
	Name       string
	MethodType string
	Path       string
	Handler    http.HandlerFunc
}

type Routes []Route

func NewRouter() *mux.Router {
	router := mux.NewRouter().StrictSlash(true)
	for _, route := range routes {
		router.
			Name(route.Name).
			Methods(route.MethodType).
			Path(route.Path).
			Handler(route.Handler)
	}
	return router
}

var routes = Routes{
	Route{
		"Home",
		"GET",
		"/",
		getHola,
	},
	Route{
		"Login",
		"POST",
		"/Login",
		Login,
	},
	Route{
		"Carga",
		"POST",
		"/Carga",
		Carga,
	},
}
