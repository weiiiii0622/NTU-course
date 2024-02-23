/****************************************************************************
  FileName      [ searchPage.js ]
  PackageName   [ src ]
  Author        [ Chin-Yi Cheng ]
  Synopsis      [ display the search result ]
  Copyright     [ 2022 11 ]
****************************************************************************/

import React, { useState, useEffect, useRef } from 'react'
import '../css/searchPage.css'
import { useNavigate, useLocation } from 'react-router-dom'

import axios from 'axios'
//import { set } from 'mongoose'
const instance = axios.create({
    baseURL: 'http://localhost:4000/api'
})

const SearchPage = () => {
    const { state } = useLocation();
    const [restaurants, setRestaurant] = useState([])

    const getRestaurant = async (priceFilter, mealFilter, typeFilter, sortBy) => {
        // TODO Part I-3-b: get information of restaurants from DB
        console.log("Getting restaurants.....");
        console.log(priceFilter, mealFilter, typeFilter, sortBy)
        try{
            const ret = await instance.get('/getSearch', {params: {
                priceFilter: priceFilter,
                mealFilter: mealFilter,
                typeFilter: typeFilter,
                sortBy: sortBy
            },});
            console.log(ret.data.contents);
            setRestaurant(ret.data.contents);
        }
        catch(error){
            console.log(error.response.data.message);
        }
    }

    useEffect(() => {
        getRestaurant(state.priceFilter, state.mealFilter, state.typeFilter, state.sortBy);
    }, [state.priceFilter, state.mealFilter, state.typeFilter, state.sortBy])


    const navigate = useNavigate();
    const ToRestaurant = (id) => {
        // TODO Part III-1: navigate the user to restaurant page with the corresponding id
        console.log(id);
        navigate('/restaurant/'+id, {
        })
    }
    const getPrice = (price) => {
        let priceText = ""
        for (let i = 0; i < price; i++)
            priceText += "$"
        return (priceText)
    }

    const parseDescription = (des) => {
        let desText = "";
        des.forEach((d, i) => {
            if(i!==des.length-1){
                desText += `${d}, `;
            }
            else{
                desText += `${d}`;
            }
        });
        return (desText);
    }

    return (

        <div className='searchPageContainer'>
            {   
                restaurants.map((item) => (
                    // TODO Part I-2: search page front-end
            
                            <>
                            <div className='resBlock' key={`${item.id}`} id={`${item.id}`} onClick={(e)=>{ToRestaurant(item.id)}}>
                                <div className='resImgContainer'>
                                    <img className='resImg' src={item.img} onClick={(e)=>{ToRestaurant(item.id)}}/>
                                </div>
                                <div className='resInfo'>
                                    <div className='title'>
                                        <p className='name'>{item.name}</p>
                                        <p className='price'>{getPrice(item.price)}</p>
                                        <p className='distance'>{`${item.distance/1000} km`}</p>
                                    </div>
                                    <p className='description'>{parseDescription(item.tag)}</p>
                                </div>
                            </div>
                            </>
                    
                ))
            }
        </div>
    )
}
export default SearchPage