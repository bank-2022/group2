const card = require("../models/cardModel");
const account = require("../models/accountModel");
const bcrypt = require("bcrypt");
const { json } = require("express/lib/response");

const getAll = (req, res) => {
    card.get(function(err,dbResult){
        if(err){
            return res.json({status:"error",message:err});
        }else{
            res.json({status:"success",message:dbResult});
        }
    });
}

const getByCardNumber = (req, res) => {

    if(req.body.card_number){

        // check if user has access to queried card
        card.getByUserID(req.userId, (err, dbResult) =>{

            if(err){
                return res.json({status:"error",message:err});
            }

            let hasAccessToCard = false;
        
            for(let i = 0; i < dbResult.length; i++){
                if(dbResult[i].card_number === req.body.card_number){
                    hasAccessToCard = true;
                }
            }

            if(!hasAccessToCard){
                return res.json({status:"error",message:"User doesn't have this card"});
            }
        })

        card.getByNumber(req.body.card_number, function(err,dbResult){
            if(err){
                return res.json({status:"error",message:err});
            }

            if(dbResult.length > 0){
                res.json({status:"success",message:dbResult});
            }else{
                res.json({status:"error",message:"Card not found"})
            }
        });

    }else{
        res.json({message:"Please fill all fields"});
    }
}

const getByUserID = (req, res) => {
    card.getByUserID(req.userId, function(err,dbResult){

        if(err){
            return res.json({status:"error",message:err})
        }

        if(dbResult.length > 0){
            res.json(dbResult);
        }else{
            res.json({status:"error",message:"No cards found for this user"})
        }

    });
}

const updateCardStatus = (req, res) => {
    if(req.body.active && req.body.card_number){

        // check if user has access to queried card
        card.getByUserID(req.userId, (err, dbResult) =>{

            if(err){
                return res.json({status:"error",message:err});
            }

            let hasAccessToCard = false;
        
            for(let i = 0; i < dbResult.length; i++){
                if(dbResult[i].card_number === req.body.card_number){
                    hasAccessToCard = true;
                }
            }

            if(!hasAccessToCard){
                return res.json({status:"error",message:"User doesn't have this card"});
            }

            // activating card, resetting tries
            if(req.body.active === 1){
                card.updateTries(0, req.body.card_number, (err, dbResult) =>{

                    if(err){
                        return res.json({status:"error",message:"Error on resetting card's tries"})
                    }

                    if(dbResult.affectedRows > 0){
                        console.log("Unlocking card, resetting tries");
                    }else{
                        console.log("Error on resetting tries");
                    }
                })
            }

            card.updateActiveStatus(req.body.active, req.body.card_number, function(err, dbResult){

                if(err){
                    return res.json({status:"error",message:err})
                }
                
                if(dbResult.affectedRows > 0){
                    res.json({status:"success",message:"Successfully modified card's status"});
                }else{
                    res.json({status:"error",message:"Card not found"})
                }
                
            });
        })
        
    }else{
        res.json({status:"error",message:"Please fill all fields"});
    }
}
/*
const addCard = (req, res) => {
    if(req.body.pin && req.body.card_number && req.body.card_type){

        let accountId = null;

        // get account id from database
        card.getByUserID(req.userId, (err, dbResult) =>{
            if(err){
                console.log(err);
            }

            if(dbResult.length > 0){
                accountId = dbResult[0].account_ID;

                card.add(req, accountId, function(err, result){
                    if(err){
                        return res.json({status:"error",message:err})
                    }else{
                        res.json({status:"success", message:"Successfully added new card"});
                    }
                });
            }
        })
        
    }else{
        res.json({message:"Please fill all fields"});
    }
}
*/

// add card to db without connecting it to any user or account
const addCard = (req, res) => {
    if(req.body.pin && req.body.card_number){
        card.add(req, function(err, dbResult){
            if(err){
                return res.json({status:"error",message:err});
            }else{
                res.json({status:"success",message:"Successfully added new card to database."});
            }
        });
    }
}

// connect account to user & account
const connectCard = (req, res) => {
    if(req.userId && req.body.accountId && req.body.card_type){
        let ctype = null;
        if(req.body.card_type === "Debit"){
            ctype = 0;
        }else if(req.body.card_type === "Credit"){
            ctype = 1;
        }else{
            return res.json({status:"error",message:"Invalid card type."});
        }
        account.getByUserId(req, function(err, dbResult){
            if(err){
                return res.json({status:"error",message:err});
            }else{
                let hasAccessToAccount = false;
                for(let i=0;i<dbResult.length;i++){
                    if(dbResult[i].account_ID === req.body.accountId){
                        hasAccessToAccount = true;
                    }
                }
                if(!hasAccessToAccount){
                    return res.json({status:"error",message:"User does not have access to this account."});
                }
                card.connectToAccount(req.body.accountId, req.userId, ctype, function(err, dbResult){
                    if(err){
                        return res.json({status:"error",message:err});
                    }else if(dbResult.affectedRows === 0){
                        return res.json({status:"error",message:"No cards available."});
                    }
                    else{
                        return res.json({status:"success",message:"Successfully added card to selected account."});
                    }
                });
            }
        });
    }else{
        return res.json({status:"error",message:"Please fill all fields."});
    }
}

const authenticate = (req, res) => {
    if(req.body.card_number && req.body.pin){
        card.getByNumber(req.body.card_number, function(err, dbResult){
            if(err){
                return res.json({status:"error",message:err})
            }

            // if card is found from database continue
            if(dbResult.length > 0){

                // if card is deactivated return
                if(dbResult[0].active === 0){
                    return res.json({status:"error",message:"Card is locked!"});
                }
                
                // if tries are >=3 and card is active then lock the card
                if(dbResult[0].tries >= 3 && dbResult[0].active === 1){
                    card.updateActiveStatus(0, req.body.card_number, (err, dbResult) =>{
                        if(err){
                            console.log(err);
                        }

                        if(dbResult.affectedRows > 0){
                            console.log("Card is deactivated");
                        }else{
                            console.log("Error on deactivating card");
                        }

                        return res.json({status:"error",message:"Card deactivated!"});
                    })
                }else{
                    // comapre request pin and db pin if they dont match increment tries
                    bcrypt.compare(req.body.pin, dbResult[0].pin.toString(), (err, match)=>{
                        if(err){
                            return res.json({status:"error",message:err})
                        }

                        if(match){
                            console.log("Successfully authenticated!");
                            return res.json({status:"success",message:"Successfully authenticated!"});
                        }else{
                            console.log("Invalid pin code or card number!");

                            let tries = dbResult[0].tries + 1;

                            card.updateTries(tries, req.body.card_number, (err, dbResult) =>{
                                if(err){
                                    console.log(err);
                                }

                                if(dbResult.affectedRows > 0){
                                    console.log("Incremented tries");
                                }else{
                                    console.log("Error on incrimenting tries");
                                }

                                res.json({status:"error",message:"Invalid pin code!"});
                            })
                        }
                    });
                }

            }else{
                res.json({status:"error",message:"Card was not found"});
            }
            
        });
    }else{
        res.json({status:"error",message:"Please fill all fields"})
    }
    
}

const deleteCard = (req, res) => {
    if(req.body.card_number){

        // check if user has access to queried card
        card.getByUserID(req.userId, (err, dbResult) =>{

            if(err){
                return res.json({status:"error",message:err});
            }

            let hasAccessToCard = false;
        
            for(let i = 0; i < dbResult.length; i++){
                if(dbResult[i].card_number === req.body.card_number){
                    hasAccessToCard = true;
                }
            }

            if(!hasAccessToCard){
                return res.json({status:"error",message:"User doesn't have this card"});
            }

            card.delete(req, function(err,result) {

                if(err){
                    return res.json({status:"error",message:err})
                }
    
                if(result.affectedRows > 0){
                    res.json({status:"success",message:"Card deleted"});
                }else{
                    res.json({status:"error",message:"Card not found"})
                }
            })
        })
        
    }else{
        res.json({status:"error",message:"Please fill all fields"})
    }
    
}

module.exports = {
    getAll,
    getByCardNumber,
    getByUserID,
    updateCardStatus,
    addCard,
    authenticate,
    deleteCard,
    connectCard
}