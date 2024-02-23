import React, {useEffect, useState, useContext, useRef} from "react";
import { TodoContext } from "../components/App";

const Footer = () => {
	
	const {todos, setTodos, mode, setMode} = useContext(TodoContext);
    const [todoCnt, setTodoCnt] = useState(0);
    const clearRef = useRef();

    useEffect(() => {
        var cnt = 0;
        for (let i = 0; i < todos.length; i++) {
            if(todos[i].isComplete === false){
                cnt += 1;
            }
        }
        if(todos.length !== 0){
            console.log(clearRef);
            if(cnt !== todos.length){
                clearRef.current.hidden = false;
            }
            else{
                clearRef.current.hidden = true;
            }
        }
        setTodoCnt(cnt);
    }, [todos]);

    const handleMode = (newMode) => {
        setMode(newMode);
    }

    const handleClear = () => {
        let newtodos = todos.filter((todo)=>todo.isComplete===false);
        setTodos([...newtodos]);
    }
    
    if(todos.length!==0){
        return(
            <>
                <footer className="todo-app__footer" id="todo-footer">
                    <div className="todo-app__total">
                        <span id="todo-count">{todoCnt} </span>
                        left
                    </div>
                    <ul className="todo-app__view-buttons">
                        <li><button id="todo-all" onClick={event => handleMode("All")}>All</button></li>
                        <li><button id="todo-active" onClick={event => handleMode("Active")}>Active</button></li>
                        <li><button id="todo-completed" onClick={event => handleMode("Completed")}>Completed</button></li>
                    </ul>
                    <div className="todo-app__clean">
                        <button id="todo-clear-complete" ref={clearRef} onClick={handleClear}>Clear completed</button>
                    </div>
                </footer> 
            </>
        );
    }
}

export default Footer;