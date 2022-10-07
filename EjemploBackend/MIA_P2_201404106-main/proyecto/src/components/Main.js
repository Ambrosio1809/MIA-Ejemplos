import React from 'react'
import {Switch,Route,Redirect} from 'react-router-dom';
import Home from './Home';
import Login from './Login';
import CargaMasiva from '../AdminComponents/CargaMasiva'




const Main =()=>{
    
        return(
            <Switch>
                <Route path="/login" component={()=><Login/>}/>
                <Route path="/carga" component={()=><CargaMasiva/>}/>
                {/*------------HOME-----------*/ }
                <Route path="/home" component={()=><Home/>}/>
                <Redirect to="/home"/>                
            </Switch>
            
        )            

}

export default Main