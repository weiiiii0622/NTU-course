/****************************************************************************
  FileName      [ Row.js ]
  PackageName   [ src/components ]
  Author        [ Cheng-Hua Lu ]
  Synopsis      [ This file generates the Row. ]
  Copyright     [ 2022 10 ]
****************************************************************************/

import "./css/Row.css";
import React from 'react';


const Row = ({ guess, rowIdx }) => {
    
    if(guess === undefined){
        return (
            <div className='Row-container'>
                {/* TODO 3: Row Implementation -- Row */}
                
                {/* ↓ Default row, you should modify it. ↓ */}
                <div className='Row-wrapper'>
                    <div id={rowIdx+'-0'} key={rowIdx+'-0'} className='Row-wordbox'></div>
                    <div id={rowIdx+'-1'} key={rowIdx+'-1'} className='Row-wordbox'></div>
                    <div id={rowIdx+'-2'} key={rowIdx+'-2'} className='Row-wordbox'></div>
                    <div id={rowIdx+'-3'} key={rowIdx+'-3'} className='Row-wordbox'></div>
                    <div id={rowIdx+'-4'} key={rowIdx+'-4'} className='Row-wordbox'></div>
                </div>
                {/* ↑ Default row, you should modify it. ↑ */}
            </div>
        )
    }
    else{
        return (
            <div className='Row-container'>
                {/* TODO 3: Row Implementation -- Row */}
                
                {/* ↓ Default row, you should modify it. ↓ */}
                <div className='Row-wrapper'>
                    <div id={rowIdx+'-0'} key={rowIdx+'-0'} className={'Row-wordbox '+guess[0].color}>{guess[0].char}</div>
                    <div id={rowIdx+'-1'} key={rowIdx+'-1'} className={'Row-wordbox '+guess[1].color}>{guess[1].char}</div>
                    <div id={rowIdx+'-2'} key={rowIdx+'-2'} className={'Row-wordbox '+guess[2].color}>{guess[2].char}</div>
                    <div id={rowIdx+'-3'} key={rowIdx+'-3'} className={'Row-wordbox '+guess[3].color}>{guess[3].char}</div>
                    <div id={rowIdx+'-4'} key={rowIdx+'-4'} className={'Row-wordbox '+guess[4].color}>{guess[4].char}</div>
                </div>
                {/* ↑ Default row, you should modify it. ↑ */}
            </div>
        )
    }
}

export default Row;