/****************************************************************************
  FileName      [ useWordle.js ]
  PackageName   [ src/components/hook ]
  Author        [ Cheng-Hua Lu ]
  Synopsis      [ This file handles each action in the Wordle game. ]
  Copyright     [ 2022 10 ]
****************************************************************************/

import React, { useState } from 'react';


const useWordle = (solution) => {
    const [turn, setTurn] = useState(0);                            // An integer whose default is 0. 0 <= turn <= 5.
    const [usedChars, setUsedChars] = useState({});                 // A dictionary object which store characters' color that showed on the keyboard. (Ex: {e: 'yellow', c:'grey'})
    const [curGuess, setCurGuess] = useState("");                   // A string whose default is "". 0 <= curGuess.length <= 5.
    const [isCorrect, setIsCorrect] = useState(false);              // A bool whose default is false. It will be set true only when curGuess === solution.
    const [guesses, setGuesses] = useState([...Array(6)]);          // An array whose length is 6. (Ex: [[{char:'c', color:'grey'},{char:'o', color:'grey'},{char:'d', color:'grey'},{char:'e', color:'yellow'},{char:'s', color:'grey'}],[],[],[],[],[]])

    // You can use this function to print all the parameters you want to know.
    const printTest = () => {
        console.log("*-----------------------*");
        console.log("solution: ", solution);
        console.log("turn: ", turn);
        console.log("usedChars:", usedChars);
        console.log("curGuess: ", curGuess);
        console.log("isCorrect: ", isCorrect);
        console.log("guesses: ", guesses);
    }

    // Handle the actions of `Enter`
    const handleEnter = () => {
        // (1) Enter is invalid if turn > 5
        if (turn > 5) {
            console.log("Error: You have used all your guesses");
            return;
        }
        // (2) Enter is invalid if curGuess is not a 5-character string
        if (curGuess.length !== 5) {
            console.log("Error: Only ", curGuess.length, " characters are entered!");
            return;
        }
        // (3) Press Enter, store curGuess to guesses, reset curGuess and update parameters .
        
        //console.log("Press Enter!!!! Store and reset curGuess!");
        // TODO 4: Check each wordbox's color in `curGuess` and update `guess`, `turn` and `curGuess`
        // Hint: check green first, and then check yellow.

        //console.log(solution);
        //console.log(guesses);
        // var cnt = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0];
        // for(let i=0; i<5; i++){
        //     cnt[solution[i].charCodeAt()-97] += 1;
        // }
        // console.log(cnt);
        var sol = [];
        var find = [0, 0, 0, 0, 0];
        var guess = [{}, {}, {}, {}, {}];
        for(let i=0; i<5; i++){
            sol.push(solution[i]);
        }
        for(let i=0; i<5; i++){
            if(curGuess[i] === sol[i]){
                guess[i] = {'char':curGuess[i], 'color':'green'};
                sol[i] = '-1';
                find[i] = 1;
            }
        }
        for(let i=0; i<5; i++){
            if(!find[i]){
                var idx = sol.indexOf(curGuess[i]);
                if(idx !== -1){
                    guess[i] = {'char':curGuess[i], 'color':'yellow'};
                    sol[idx] = '-1';
                }
                else{
                    guess[i] = {'char':curGuess[i], 'color':'grey'};
                }
                find[i] = 1;
            }
        }
        

        // for(let i=0; i<5; i++){
        //     var c = curGuess[i];
        //     if(cnt[c.charCodeAt()-97] > 0){
        //         cnt[c.charCodeAt()-97] -= 1;
        //         if(curGuess[i] === solution[i]){
        //             guess.push({'char':c, 'color':'green'});
        //         }
        //         else{
        //             guess.push({'char':c, 'color':'yellow'});
        //         }
        //     }
        //     else{
        //         guess.push({'char':c, 'color':'grey'});
        //     }
        // }
        //console.log(guess);
        // add the formatted guess generated into guesses.
        var newGuesses = [];
        for(let i=0; i<6; i++){
            if(i === turn){
                newGuesses.push(guess);
            }
            else{
                newGuesses.push(guesses[i]);
            }
        }
        //console.log(newGuesses);
        setGuesses([...newGuesses]);
        
        // turn += 1
        setTurn(turn+1);
        // set curGuess to default
        

        // TODO 5: update parameters, check each char usage and show in `Keyboard` and reset `curGuess`.
        // 5-1) check if curGuess === solution, if true, set `isCorrect` to true.
        if(curGuess === solution){
            setIsCorrect(true);
        }
        
        // 5-2) usedChars update
        // console.log(usedChars);
        let newUsedChars = JSON.parse(JSON.stringify(usedChars));
        for(let i=0; i<5; i++){
            if(newUsedChars[guess[i].char] === 'green'){
                continue;
            }
            else if((newUsedChars[guess[i].char] === 'yellow'||newUsedChars[guess[i].char] === undefined) && guess[i].color === 'green'){
                newUsedChars[guess[i].char] = guess[i].color;
            }
            else if(guess[i].color === 'yellow'){
                newUsedChars[guess[i].char] = guess[i].color;
            }
            
        }
        console.log(newUsedChars);
        setUsedChars(newUsedChars);
        setCurGuess("");
    }

    // Handle the action of `Backspace`
    const handleBackspace = () => {
        setCurGuess(curGuess.substring(0, curGuess.length - 1));
    }

    // Handle the action of pressing a character.
    const handleCharacter = (key) => {
        // If curGuess's length is longer than 5, do nothing
        if (curGuess.length < 5){
            setCurGuess(curGuess + key);
        }
    }
    const handleKeyup = ({ key }) => {
        // console.log("You just press: ", key);
        if (key === 'Enter') handleEnter();
        else if (key === 'Backspace') handleBackspace();
        else if (/^[A-Za-z]$/.test(key)) handleCharacter(key);
    }
    return { turn, curGuess, guesses, isCorrect, usedChars, handleKeyup, printTest };
}

export default useWordle;
