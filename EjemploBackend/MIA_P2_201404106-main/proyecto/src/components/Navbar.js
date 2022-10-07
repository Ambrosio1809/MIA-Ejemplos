import React from 'react'
import {Nav, Navbar} from 'react-bootstrap';
import 'bootstrap/dist/css/bootstrap.min.css';
import Cookies from 'universal-cookie'
const cookies = new Cookies();

class Navegacion extends React.Component{


    CerrarSesion =()=>{
        cookies.remove('id',{path:"/"})
        cookies.remove('Usuario',{path:"/"})
        cookies.remove('Nombre',{path:"/"})
        cookies.remove('Apellido',{path:"/"})
        window.location.href="/login"

    }

    render(){
        return(
            <Navbar bg="dark" variant ="dark" expand="lg">
                <Navbar.Brand href="/login">Ejemplo</Navbar.Brand>
                <Navbar.Toggle aria-controls="basic-navbar-nav" />
                <Navbar.Collapse id="basic-navbar-nav">
                    <Nav className="mr-auto">

                    </Nav>              
                </Navbar.Collapse>
            </Navbar>

        );

    }
        
    

}

export default Navegacion;