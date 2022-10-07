import React from 'react';
import {Form,Button} from 'react-bootstrap';
import NavegacionInit from './Navbar1';

class Login extends React.Component{
    state ={
        form:{
            Username:'',
            Password:'',
        },
        id:""       
    }
 
    handleChange = e =>{
        this.setState({
            form:{
                ...this.state.form,
                [e.target.name]:e.target.value
            }
        })
    }

    handleSubmit = async e =>{
        e.preventDefault()
        console.log(this.state.form)

       try{
            let config ={
                method: 'POST',
                headers:{
                    'Accept': 'application/json',
                    'Content-Type':'application/json'
                },
                body: JSON.stringify(this.state.form)
                
            }
            await fetch('http://localhost:4000/Login',config)
            .then(res=>res.json())
            .then(Response => {
                console.log(Response);                    
                      
            })
            .catch(error=>{
                console.log(error)
            })       

        }catch(error){
            console.log(error);
            
        }

    }

    render(){
        return(
            <div>
                <NavegacionInit/>
                <div className="main-container-Login col-12" align="center">
                <h1>Inicio de Sesion</h1>
                <Form onSubmit ={this.handleSubmit}>
                    <Form.Group controlId="formUsename" align ="center">
                        <Form.Label fontWeight="bold">Username</Form.Label>
                        <Form.Control type="text" name="Username" placeholder="Ingrese Usuario" onChange={this.handleChange} value={this.state.form.Username}/>                        
                    </Form.Group>
                    <Form.Group controlId="formPassword" align="center">
                        <Form.Label>Password</Form.Label>
                        <Form.Control type="password" name="Password" placeholder="Password" onChange={this.handleChange} value={this.state.form.Password}/>
                    </Form.Group>
                    
                    <Form.Group>
                        <Button variant="primary" type="submit" onSubmit={this.handleSubmit}>
                            Iniciar Sesion
                        </Button>
                    </Form.Group>                    
                </Form>
                </div>

            </div>
            

        );
    }

}
export default Login;