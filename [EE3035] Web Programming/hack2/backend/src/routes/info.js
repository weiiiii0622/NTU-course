// * ////////////////////////////////////////////////////////////////////////
// *
// * FileName     [ info.js ]
// * PackageName  [ server ]
// * Synopsis     [ Get restaurant info from database ]
// * Author       [ Chin-Yi Cheng ]
// * Copyright    [ 2022 11 ]
// *
// * ////////////////////////////////////////////////////////////////////////

import Info from '../models/info'

exports.GetSearch = async (req, res) => {
    /*******    NOTE: DO NOT MODIFY   *******/
    const priceFilter = req.query.priceFilter
    const mealFilter  = req.query.mealFilter
    const typeFilter  = req.query.typeFilter
    const sortBy      = req.query.sortBy
    /****************************************/

    // NOTE Hint: 
    // use `db.collection.find({condition}).exec(err, data) {...}`
    // When success, 
    //   do `res.status(200).send({ message: 'success', contents: ... })`
    // When fail,
    //   do `res.status(403).send({ message: 'error', contents: ... })` 

    // TODO Part I-3-a: find the information to all restaurants
    // console.log("init filter:");
    // console.log(priceFilter);
    // console.log(mealFilter);
    // console.log(typeFilter);
    // console.log("End init");
    let priceNum = [];

    priceFilter?.forEach(element => {
        if(element === '$') priceNum.push(1);
        else if(element === '$$') priceNum.push(2);
        else if(element === '$$$') priceNum.push(3);
    });
    //console.log(priceNum);

    if(priceFilter===undefined && mealFilter===undefined && typeFilter===undefined){
        if(sortBy==="price"){
            Info.find().sort({"price":1}).exec( (err, data) => {
                //console.log(data);
                if(data){
                    res.status(200).send({ message: 'success', contents: data })
                }
                else{
                    res.status(403).send({ message: 'error', contents: [] });
                }
            });
        }
        else{
            Info.find().sort({"distance":1}).exec( (err, data) => {
                //console.log(data);
                if(data){
                    res.status(200).send({ message: 'success', contents: data })
                }
                else{
                    res.status(403).send({ message: 'error', contents: [] });
                }
            });
        }
    }
    else{
        let mealMFilter = [];
        let typeMFilter = [];
        if(priceFilter===undefined) {
            // priceNum.push(1);priceNum.push(2);priceNum.push(3);
        }
        if(mealFilter===undefined) {
            // mealMFilter.push('Breakfast');
            // mealMFilter.push('Lunch');
            // mealMFilter.push('Dinner');
        }
        else{
            mealMFilter = mealFilter;
        }
        if(typeFilter === undefined){
            // typeMFilter.push('Chinese');
            // typeMFilter.push('American');
            // typeMFilter.push('Italian');
            // typeMFilter.push('Japanese');
            // typeMFilter.push('Korean');
            // typeMFilter.push('Thai');
        }
        else{
            typeMFilter = typeFilter;
        }
        console.log(mealFilter);
        console.log(mealMFilter);
        console.log(typeMFilter);
        var test = ["Lunch", "Dinner"];
        if(sortBy==="price"){
            if(priceFilter===undefined){
                if(mealFilter===undefined){
                    Info.find({$and:[{"tag": { "$in": typeMFilter }}]  } ).sort({"price":1}).exec( (err, data) => {
                        //console.log(data);
                        if(data){
                            res.status(200).send({ message: 'success', contents: data })
                        }
                        else{
                            res.status(403).send({ message: 'error', contents: [] });
                        }
                    });                   
                }
                else if(typeFilter===undefined){
                    Info.find({$and:[{"tag": { "$in": mealMFilter }}]  } ).sort({"price":1}).exec( (err, data) => {
                        //console.log(data);
                        if(data){
                            res.status(200).send({ message: 'success', contents: data })
                        }
                        else{
                            res.status(403).send({ message: 'error', contents: [] });
                        }
                    });
                }
                else{
                    Info.find({$and:[ {"tag": { "$in": mealMFilter }}, {"tag": { "$in": typeMFilter }}]  } ).sort({"price":1}).exec( (err, data) => {
                        //console.log(data);
                        if(data){
                            res.status(200).send({ message: 'success', contents: data })
                        }
                        else{
                            res.status(403).send({ message: 'error', contents: [] });
                        }
                    });
                }
            }
            else{
                if(typeFilter===undefined && mealFilter===undefined){
                    Info.find({$and:[{"price":{"$in": priceNum}}]  } ).sort({"price":1}).exec( (err, data) => {
                        //console.log(data);
                        if(data){
                            res.status(200).send({ message: 'success', contents: data })
                        }
                        else{
                            res.status(403).send({ message: 'error', contents: [] });
                        }
                    });
                }
                else if(mealFilter===undefined){
                    Info.find({$and:[{"price":{"$in": priceNum}}, {"tag": { "$in": typeMFilter }}]  } ).sort({"price":1}).exec( (err, data) => {
                        //console.log(data);
                        if(data){
                            res.status(200).send({ message: 'success', contents: data })
                        }
                        else{
                            res.status(403).send({ message: 'error', contents: [] });
                        }
                    });
                }
                else if(typeFilter === undefined){
                    Info.find({$and:[{"price":{"$in": priceNum}}, {"tag": { "$in": mealMFilter }}]  } ).sort({"price":1}).exec( (err, data) => {
                        //console.log(data);
                        if(data){
                            res.status(200).send({ message: 'success', contents: data })
                        }
                        else{
                            res.status(403).send({ message: 'error', contents: [] });
                        }
                    });
                }
                else{
                    Info.find({$and:[{"price":{"$in": priceNum}}, {"tag": { "$in": mealMFilter }}, {"tag": { "$in": typeMFilter }}]  } ).sort({"price":1}).exec( (err, data) => {
                        //console.log(data);
                        if(data){
                            res.status(200).send({ message: 'success', contents: data })
                        }
                        else{
                            res.status(403).send({ message: 'error', contents: [] });
                        }
                    });
                }

            }

        }
        else{
            if(priceFilter===undefined){
                if(mealFilter===undefined){
                    Info.find({$and:[{"tag": { "$in": typeMFilter }}]  } ).sort({"distance":1}).exec( (err, data) => {
                        //console.log(data);
                        if(data){
                            res.status(200).send({ message: 'success', contents: data })
                        }
                        else{
                            res.status(403).send({ message: 'error', contents: [] });
                        }
                    });                   
                }
                else if(typeFilter===undefined){
                    Info.find({$and:[{"tag": { "$in": mealMFilter }}]  } ).sort({"distance":1}).exec( (err, data) => {
                        //console.log(data);
                        if(data){
                            res.status(200).send({ message: 'success', contents: data })
                        }
                        else{
                            res.status(403).send({ message: 'error', contents: [] });
                        }
                    });
                }
                else{
                    Info.find({$and:[ {"tag": { "$in": mealMFilter }}, {"tag": { "$in": typeMFilter }}]  } ).sort({"distance":1}).exec( (err, data) => {
                        //console.log(data);
                        if(data){
                            res.status(200).send({ message: 'success', contents: data })
                        }
                        else{
                            res.status(403).send({ message: 'error', contents: [] });
                        }
                    });
                }
            }
            else{
                if(typeFilter===undefined && mealFilter===undefined){
                    Info.find({$and:[{"price":{"$in": priceNum}}]  } ).sort({"distance":1}).exec( (err, data) => {
                        //console.log(data);
                        if(data){
                            res.status(200).send({ message: 'success', contents: data })
                        }
                        else{
                            res.status(403).send({ message: 'error', contents: [] });
                        }
                    });
                }
                else if(mealFilter===undefined){
                    Info.find({$and:[{"price":{"$in": priceNum}}, {"tag": { "$in": typeMFilter }}]  } ).sort({"distance":1}).exec( (err, data) => {
                        //console.log(data);
                        if(data){
                            res.status(200).send({ message: 'success', contents: data })
                        }
                        else{
                            res.status(403).send({ message: 'error', contents: [] });
                        }
                    });
                }
                else if(typeFilter === undefined){
                    Info.find({$and:[{"price":{"$in": priceNum}}, {"tag": { "$in": mealMFilter }}]  } ).sort({"distance":1}).exec( (err, data) => {
                        //console.log(data);
                        if(data){
                            res.status(200).send({ message: 'success', contents: data })
                        }
                        else{
                            res.status(403).send({ message: 'error', contents: [] });
                        }
                    });
                }
                else{
                    Info.find({$and:[{"price":{"$in": priceNum}}, {"tag": { "$in": mealMFilter }}, {"tag": { "$in": typeMFilter }}]  } ).sort({"distance":1}).exec( (err, data) => {
                        //console.log(data);
                        if(data){
                            res.status(200).send({ message: 'success', contents: data })
                        }
                        else{
                            res.status(403).send({ message: 'error', contents: [] });
                        }
                    });
                }

            }
        }        
    }
    
    // TODO Part II-2-a: revise the route so that the result is filtered with priceFilter, mealFilter and typeFilter
    
    // TODO Part II-2-b: revise the route so that the result is sorted by sortBy
}

exports.GetInfo = async (req, res) => {
    /*******    NOTE: DO NOT MODIFY   *******/
    const id = req.query.id
    /****************************************/

    // NOTE USE THE FOLLOWING FORMAT. Send type should be 
    // if success:
    // {
    //    message: 'success'
    //    contents: the data to be sent. Hint: A dictionary of the restaruant's information.
    // }
    // else:
    // {
    //    message: 'error'
    //    contents: []
    // }

    // TODO Part III-2: find the information to the restaurant with the id that the user requests
    console.log("Find info id: "+id);
    Info.find({"id": id}).exec( (err, data) => {
        //console.log(data);
        if(data){
            res.status(200).send({ message: 'success', contents: data})
        }
        else{
            res.status(403).send({ message: 'error', contents: [] });
        }
    });
}