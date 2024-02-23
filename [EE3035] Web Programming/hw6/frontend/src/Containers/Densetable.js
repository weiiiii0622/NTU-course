import * as React from 'react';
import { Table, TableBody, TableCell, TableContainer, TableHead, TableRow } from '@material-ui/core';
import { Paper } from '@material-ui/core';

export default function DenseTable({rows}) {
    return (
        <TableContainer component={Paper}>
        <Table sx={{ minWidth: 650 }} size="small" aria-label="a dense table">
            {
                rows.length!==0 && 
                <TableHead>
                <TableRow>
                    <TableCell>Name</TableCell>
                    <TableCell align="right">Subject</TableCell>
                    <TableCell align="right">Score</TableCell>
                </TableRow>
                </TableHead>
            }
            <TableBody>
            {rows.map((row) => (
                <TableRow
                key={row.name}
                sx={{ '&:last-child td, &:last-child th': { border: 0 } }}
                >
                <TableCell component="th" scope="row">
                    {row.name}
                </TableCell>
                <TableCell align="right">{row.subject}</TableCell>
                <TableCell align="right">{row.score}</TableCell>
                </TableRow>
            ))}
            </TableBody>
        </Table>
        </TableContainer>
    );
}