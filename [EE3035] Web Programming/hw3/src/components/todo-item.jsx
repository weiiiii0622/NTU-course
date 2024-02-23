import React , {useRef, useEffect, useContext} from "react";
import x from './x.png'
import { TodoContext } from "../components/App";


const TodoItem = () =>{
    const {todos, setTodos, mode} = useContext(TodoContext);

    let handleOnClick = (e) => {
        var idx = e.target.id.slice(5)-'0';
        var newtodos = [...todos];
        newtodos[idx].isComplete = !newtodos[idx].isComplete;
        setTodos([...newtodos]);
    }

    let handleOnDelete = (e) =>{
        var idx = e.target.id.slice(5)-'0';
        let newtodos = todos.filter((todo, index)=>index!==(idx));
        setTodos([...newtodos]);
    }

    if(todos.length !== 0){
        console.log(mode);
        return (
            todos.filter((todo)=>{
                if(mode === "All"){
                    return todo;
                }
                else if(mode === "Active"){
                    return todo.isComplete===false;
                }
                else if(mode === "Completed"){
                    return todo.isComplete===true;
                }
                else{
                    return todo;
                }
            }).map((todo, i) =>
                (<li className="todo-app__item" key={"todo_"+i}>
                    <div className="todo-app__checkbox">
                        <input id={"todo_"+i} type="checkbox" onClick={handleOnClick} checked={todo.isComplete} readOnly/>
                        <label htmlFor={"todo_"+i}></label>
                    </div>
                    <h1 className="todo-app__item-detail" style={todo.isComplete?{textDecoration:"line-through", opacity:"0.5"}:{}}>
                        {todo.detail}
                    </h1>
                    <img id={"todo_"+i} src={x} alt="" className="todo-app__item-x" onClick={handleOnDelete}/>
                </li>)
            )
        );
    }
}

export default TodoItem