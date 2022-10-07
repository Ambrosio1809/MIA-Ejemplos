import React from 'react';
import {withRouter} from 'react-router-dom'

class Home extends React.Component{
    
    onSubmit = async (e) =>{
        e.preventDefault();
        this.props.history.push('/login')
     }

    render (){

        return(
            <div className="main-container col-12" align="center">
                <h1>
                    Bienvenido a Quiniela App
                </h1>
                <h3>Desea ingresar a la aplicacion</h3>
                <button variant="primary" type="submit" onClick={(e)=>this.onSubmit(e)}>
                    Ingresar
                </button>               

            </div>
        )

    }

}
export default withRouter(Home)