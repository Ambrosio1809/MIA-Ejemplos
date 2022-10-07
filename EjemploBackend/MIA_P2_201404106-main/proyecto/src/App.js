import React from 'react';
import './App.css';

import Main from './components/Main'

import {BrowserRouter as Route} from 'react-router-dom';

function App(){

  return(
    <Route>
      <div className="App">
        <Main/>
      </div>
    </Route>
  )
}

export default App;
