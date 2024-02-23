/****************************************************************************
  FileName      [ restaurantPage.js ]
  PackageName   [ src ]
  Author        [ Chin-Yi Cheng ]
  Synopsis      [ Implement the restaurant page ]
  Copyright     [ 2022 11 ]
****************************************************************************/

import React, { useState, useEffect } from 'react'
import '../css/restaurantPage.css'
import Information from './information';
import Comment from './comment';
import { useParams } from 'react-router-dom'

import axios from 'axios'
const instance = axios.create({
    baseURL: 'http://localhost:4000/api'
})

const RestaurantPage = () => {
    const { id } = useParams()
    const [info, setInfo] = useState({})
    const [comments, setComments] = useState([])
    const [loading, setLoading] = useState(true)
    const getInfo = async () => {
        // TODO Part III-2: get a restaurant's info
        console.log("Getting restaurant info.....");
        try{
            const ret = await instance.get('/getInfo', {params: {
                id: id
            },});
            console.log(ret.data.contents[0]);
            setInfo(ret.data.contents[0]);
        }
        catch(error){
            console.log(error.response.data.message);
        }
    }
    const getComments = async () => {
        // TODO Part III-3: get a restaurant's comments 
        console.log("Getting restaurant comment....."+id);
        try{
            const ret = await instance.get('/getCommentsByRestaurantId', {params: {
                "restaurantId": id
            },});
            console.log(ret.data.contents);
            setComments([...ret.data.contents]);
        }
        catch(error){
            console.log(error.response.data.message);
        }
    }
    useEffect(() => {
        if (Object.keys(info).length === 0) {
            getInfo()
            getComments();
        }
    }, [])
    
    useEffect(() => {
        // TODO Part III-3-c: update the comment display immediately after submission
        if(loading===false) getComments();
        setLoading(true);
    }, [loading]) //message

    /* TODO Part III-2-b: calculate the average rating of the restaurant */
    let rating = 0;
    if(comments.length > 0){
        comments?.forEach((c, i) => {
            rating += c.rating;
        })
        rating /= comments.length;
    }

    return (
        <div className='restaurantPageContainer'>
            {Object.keys(info).length === 0 ? <></> : <Information info={info} rating={rating} />}
            <Comment restaurantId={id} comments={comments} setComments={setComments} setLoad={setLoading} />
        </div>
    )
}
export default RestaurantPage