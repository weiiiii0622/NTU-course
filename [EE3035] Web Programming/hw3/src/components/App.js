import './App.css';
import React, {useState, createContext} from 'react';
import Header from '../containers/header';
import Main from '../containers/Main';
import Footer from '../containers/footer';

export const TodoContext = createContext();

const App = () =>{
  const [todos, setTodos] = useState([]);
  const [mode, setMode] = useState("All");

  return (
    <div className="todo-app__root">
      <TodoContext.Provider value={{todos, setTodos, mode, setMode}}>
      <Header/>
      <Main />
      <Footer />
      </TodoContext.Provider>
    </div>
  );
}

export default App;
