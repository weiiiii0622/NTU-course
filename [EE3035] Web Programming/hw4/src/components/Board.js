/****************************************************************************
  FileName      [ Board.js ]
  PackageName   [ src/components ]
  Author        [ Cheng-Hua Lu ]
  Synopsis      [ This file generates the Board. ]
  Copyright     [ 2022 10 ]
****************************************************************************/

import './css/Board.css'
import Cell from './Cell';
import Modal from './Modal';
import Dashboard from './Dashboard';
import { revealed } from '../util/reveal';
import createBoard from '../util/createBoard';
import React, { useEffect, useState } from 'react';


const Board = ({ boardSize, mineNum, backToHome }) => {
    const [board, setBoard] = useState([]);                     // An 2-dimentional array. It is used to store the board.
    const [nonMineCount, setNonMineCount] = useState(0);        // An integer variable to store the number of cells whose value are not '💣'.
    const [mineLocations, setMineLocations] = useState([]);     // An array to store all the coordinate of '💣'.
    const [gameOver, setGameOver] = useState(false);            // A boolean variable. If true, means you lose the game (Game over).
    const [remainFlagNum, setRemainFlagNum] = useState(0);      // An integer variable to store the number of remain flags.
    const [win, setWin] = useState(false);                      // A boolean variable. If true, means that you win the game.

    useEffect(() => {
        // Calling the function
        freshBoard();
    }, []);

    // Creating a board
    const freshBoard = () => {
        const newBoard = createBoard(boardSize, mineNum);
        // Basic TODO: Use `newBoard` created above to set the `Board`.
        // Hint: Read the definition of those Hook useState functions and make good use of them.

        setBoard(newBoard.board);
        setNonMineCount(boardSize*boardSize - newBoard.mineLocations.length);
        setMineLocations(newBoard.mineLocations);
        setRemainFlagNum(newBoard.mineLocations.length);

    }

    const restartGame = () => {
        freshBoard();
        setGameOver(false);
        setWin(false);
    }

    // On Right Click / Flag Cell
    const updateFlag = (e, x, y) => {
        // To not have a dropdown on right click
        e.preventDefault();
        if(remainFlagNum === 0) return;
        // Deep copy of a state
        let newBoard = JSON.parse(JSON.stringify(board));
        let newFlagNum = remainFlagNum;

        // Basic TODO: Right Click to add a flag on board[x][y]
        // Remember to check if board[x][y] is able to add a flag (remainFlagNum, board[x][y].revealed)
        // Update board and remainFlagNum in the end

        if(!newBoard[x][y].revealed && !newBoard[x][y].flagged){
            newBoard[x][y].flagged = true;
            newFlagNum -= 1;
        }
        else if(newBoard[x][y].flagged){
            newBoard[x][y].flagged = false;
            newFlagNum += 1;
        }

        setBoard(newBoard);
        setRemainFlagNum(newFlagNum);
    };

    const revealCell = (x, y) => {
        if (board[x][y].revealed || gameOver || board[x][y].flagged) return;
        let newBoard = JSON.parse(JSON.stringify(board));
        // Basic TODO: Complete the conditions of revealCell (Refer to reveal.js)
        // Hint: If `Hit the mine`, check ...?
        //       Else if `Reveal the number cell`, check ...?
        // Reminder: Also remember to handle the condition that after you reveal this cell then you win the game.
        
        if(newBoard[x][y].value === '💣'){
            mineLocations.forEach(coord => {
                newBoard[coord[0]][coord[1]].revealed = true;
            });
            setBoard(newBoard);
            setGameOver(true);
        }
        else{
            var newNonMinesCount = nonMineCount;
            let ret = revealed(newBoard, x, y, newNonMinesCount);
            setBoard(ret.board);
            setNonMineCount(ret.newNonMinesCount);
            if(ret.newNonMinesCount === 0){
                setWin(true);
                setGameOver(true);
            }
        }

    };

    return (
        <div className='boardPage' >
            <div className='boardWrapper' >
                {/* <h1>This is the board Page!</h1>  This line of code is just for testing. Please delete it if you finish this function. */}

                {/* Advanced TODO: Implement Modal based on the state of `gameOver` */}

                {(gameOver || win) && <Modal restartGame={restartGame} backToHome={backToHome} win={win}/>}

                {/* Basic TODO: Implement Board 
                Useful Hint: The board is composed of BOARDSIZE*BOARDSIZE of Cell (2-dimention). So, nested 'map' is needed to implement the board.
                Reminder: Remember to use the component <Cell> and <Dashboard>. See Cell.js and Dashboard.js for detailed information. */}

                <div className='boardContainer'>
                    <Dashboard remainFlagNum={remainFlagNum} gameOver={gameOver} />
                    {
                        board.map((row, index) => {
                            return (
                                <div className={`row${index}`} style={{display: `flex`}}>
                                    {
                                        row.map((col, index) => {
                                            return (
                                                <Cell rowIdx={col.x} colIdx={col.y} detail={col} updateFlag={updateFlag} revealCell={revealCell}/>
                                            )
                                        })
                                    }
                                </div>
                            )
                        })
                    }
                </div>
                
            </div>
        </div>
    );
}

export default Board