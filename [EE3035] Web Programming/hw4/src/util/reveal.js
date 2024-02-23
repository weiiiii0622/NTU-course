/****************************************************************************
  FileName      [ reveal.js ]
  PackageName   [ src/util ]
  Author        [ Cheng-Hua Lu ]
  Synopsis      [ This file states the reaction when left clicking a cell. ]
  Copyright     [ 2022 10 ]
****************************************************************************/

const dx = [-1, -1, -1, 0, 0, 1, 1, 1];
const dy = [-1, 0, 1, -1, 1, -1, 0, 1];

export const revealed = (board, x, y, newNonMinesCount) => {
	let boardSize = board[0].length;
    var visited = Array(boardSize).fill().map(() => Array(boardSize).fill(false));

    // Advanced TODO: reveal cells in a more intellectual way.
    // Useful Hint: If the cell is already revealed, do nothing.
    //              If the value of the cell is not 0, only show the cell value.
    //              If the value of the cell is 0, we should try to find the value of adjacent cells until the value we found is not 0.
    //              The input variables 'newNonMinesCount' and 'board' may be changed in this function.
    if(board[x][y].value !== 0){
        board[x][y].revealed = true;
        newNonMinesCount--;
    }
	else{
		let s = [];
		s.push({x:x,y:y});
		while(s.length !== 0){
			var cur = s.shift();
			if(visited[cur.x][cur.y] === true){
				continue;
			}

			visited[cur.x][cur.y] = true;
			if(board[cur.x][cur.y].value !== '💣' && !board[cur.x][cur.y].flagged){
				board[cur.x][cur.y].revealed = true;
        		newNonMinesCount--;
			}
			if(board[cur.x][cur.y].value === 0){
				
				for(var i=0; i<8; i++){
					if(cur.x+dx[i]>=0 && cur.x+dx[i]<boardSize && cur.y+dy[i]>=0 && cur.y+dy[i]<boardSize){
						if(visited[cur.x+dx[i]][cur.y+dy[i]] === false){
							s.push({x:cur.x+dx[i], y:cur.y+dy[i]});
						}
					}
				}
			}
		}
	}

    return { board, newNonMinesCount };
};
