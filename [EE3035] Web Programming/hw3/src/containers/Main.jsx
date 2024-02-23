import React, {useState, useContext} from "react";
import TodoItem from "../components/todo-item";
import { TodoContext } from "../components/App";

const Main = () => {
	
	const {todos, setTodos, mode, setMode} = useContext(TodoContext);

	let addTodo = (e) =>{
		
		if(e.key==="Enter"){
			if(e.target.value.length !== 0){
				var newTodo = [...todos];
				newTodo.push({
					detail: e.target.value,
					isComplete: false,
				});
				setTodos(newTodo);
				e.target.value = "";
			}
		}
	}

	return(
		<>
			<section className="todo-app__main">
				<input onKeyUp={addTodo} id="todo-input" className="todo-app__input" placeholder="What needs to be done?" />
				<ul id="todo-list" className="todo-app__list">
				<TodoItem/>
				</ul>
			</section>
		</>
	);
}

export default Main;