import React, {useState} from 'react';
import NavbarA from './NavbarA';

function CargaMasiva (){

    

    const [archivos, setArchivos]=useState(null);

    const subirArchivos=e=>{
        setArchivos(e);
    }

    const insertarArchivos=async()=>{
        const f = new FormData();

        for (let index = 0; index < archivos.length; index ++){
            f.append("files", archivos[index]);            
        }
        try{
            let config ={
                method: 'POST',
                headers:{ },
                body: f
                
            }
            await fetch('http://18.217.252.94:4000/Carga',config)
            .then(response=>{
                console.log(response.data);
            }).catch(error=>{
                console.log(error)
            })

        }catch(error){

        }

    }

    
        return(
            <div>
                <NavbarA/>
                <div className="main-container col-12">
                    <h1>
                        CargaMasiva
                    </h1>
                    <input type="file" name ="files" multiple onChange={(e)=>subirArchivos(e.target.files)}/>
                    <button className="btn btn-primary" onClick={()=>insertarArchivos()}>Cargar Archivo</button>


                </div>
            </div>
        )
    
}

export default CargaMasiva;