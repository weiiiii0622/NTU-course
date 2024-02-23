/****************************************************************************
  FileName      [ information.js ]
  PackageName   [ src ]
  Author        [ Chin-Yi Cheng ]
  Synopsis      [ display the information of restaurant ]
  Copyright     [ 2022 11 ]
****************************************************************************/

import React from 'react'
import Stars from '../components/stars';
import '../css/restaurantPage.css'

const Information = ({ info, rating }) => {

    const getTag = (tags) => {
        return (
            <>
                {/* TODO Part III-2-a render tags */
                    tags.map((tag, i) => {
                        return (
                            <div className='tag' key={tag}>{tag}</div>
                        )
                    })
                }
            </>
        )
    }
    const getPriceTag = (price) => {
        let priceText = ""
        for (let i = 0; i < price; i++)
            priceText += "$"
        return (
            <>
                {/* TODO Part III-2-a render price tags; hint: convert price number to dollar signs first */
                    <div className='tag' key={priceText}>{priceText}</div>
                }
            </>
        )
    }

    const getBusiness = (time) => {
        let type = -1;
        let name = ["Mon", "Tue", "Wed", "Thr", "Fri", "Sat", "Sun"];
        let t = ["Closed","Closed","Closed","Closed","Closed","Closed","Closed"];
        console.log(time["Mon"]);
        if(time["All"] === undefined) type = 0;
        else type = 1;

        if(type===0){
            if(time["Mon"] !== undefined) t[0] = time["Mon"];
            if(time["Tue"] !== undefined) t[1] = time["Tue"];
            if(time["Wed"] !== undefined) t[2] = time["Wed"];
            if(time["Thr"] !== undefined) t[3] = time["Thr"];
            if(time["Fri"] !== undefined) t[4] = time["Fri"];
            if(time["Sat"] !== undefined) t[5] = time["Sat"];
            if(time["Sun"] !== undefined) t[6] = time["Sun"];
        }
        else{
            t = [time["All"],time["All"],time["All"],time["All"],time["All"],time["All"],time["All"]];
        }
        console.log(t);
        return (
            <div className='businessTime'>
                {/* TODO Part III-2-c: render business time for each day*/
                    t.map((item, i) => {
                        return (
                            <div className='singleDay'>
                                <div className='day'>{name[i]}</div>
                                <div className='time'>{t[i]}</div>
                            </div>
                        )
                    })
                }
            </div>
        )
    }

    return (
        <div className='infoContainer'>
            <h2>{info.name}</h2>
            <div className='infoRow'>
                <div className='rate'>
                    {rating === 0 ? <p>No Rating</p> : <Stars rating={rating} displayScore={true} />}

                </div>
                <div className='distance'>{info.distance / 1000} km</div>
            </div>
            <div className='infoRow'>
                {getPriceTag(info.price)}
                {getTag(info.tag)}
            </div>
            <h5>Business hours:</h5>
            {getBusiness(info.time)}
        </div>
    )
}
export default Information