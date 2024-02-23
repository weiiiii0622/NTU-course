import './App.css';
import React, { useState } from 'react';
import { guess, startGame, restart } from './axios';


function App() {
	const [hasStarted, setHasStarted] = useState(false);
	const [hasWon, setHasWon] = useState(false);
	const [number, setNumber] = useState('');
	const [status, setStatus] = useState('');

	const handleGuess = async () => {
		const response = await guess(number);
		if(response === 'Equal') setHasWon(true);
		else{
			setStatus(response);
			setNumber(number);
		}
	}

	const handleStartGame = async () => {
		const response = await startGame();

		setHasStarted(true);
	}

	const handleRestart = async () => {
		const response = await restart();
		setStatus('');
		setNumber('');
		setHasWon(false);
	}

	const gameMode = (
		<>
			<p>Guess a number between 1 to 100</p>
			<input onChange={(e)=>setNumber(`${e.target.value}`)}></input>
			<button // Send number to backend
				onClick={handleGuess}
				disabled={!number}
			>Guess!</button>
			<p>{status}</p>
		</>
	)

	const winningMode = (
		<>
			<p>You won! The number was {number}.</p>
			<button className='button' onClick={handleRestart}>Restart</button>
		</>
	)	

	const startMenu = (
		<div>
			<h1>Guess the Number</h1>
			<button className='button' onClick={handleStartGame}>Start Game</button>
		</div>
	)

	const game = (
		<div>
			{hasWon ? winningMode : gameMode}
		</div>
	)

	return (
		<div className="App">
			{hasStarted ? game : startMenu}
		</div>
	);
}

export default App;
