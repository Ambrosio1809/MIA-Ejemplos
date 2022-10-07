import React from 'react'
import {Nav, Navbar} from 'react-bootstrap';
import 'bootstrap/dist/css/bootstrap.min.css';

const NavegacionInit =() =>{


    
        return(
            <Navbar bg="dark" variant ="dark" expand="lg">
                <Navbar.Brand href="/login">Ejemplo</Navbar.Brand>
                <Navbar.Toggle aria-controls="basic-navbar-nav" />
                <Navbar.Collapse id="basic-navbar-nav">
                    <Nav className="mr-auto">
                        <Nav.Link href="/carga">CargaMasiva</Nav.Link>
                    </Nav>                    
                </Navbar.Collapse>
            </Navbar>

        );
    

}

export default NavegacionInit;