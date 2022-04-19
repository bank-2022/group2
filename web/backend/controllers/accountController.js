const emailvalidator = require("email-validator");
const sanitizer = require("sanitizer");
const { json } = require("express/lib/response");
const account = require("../models/accountModel");
const user = require("../models/userModel");
const card = require("../models/cardModel");

const getAll = (req, res) => {
    account.get(function(err, dbResult){
        if(err){
            return res.json(err);
        }else{
            return res.json(dbResult);
        }
    });
}

const getById = (req, res) => {
    account.getById(req.params.id,function(err, dbResult){
        if(err){
            return res.json(err);
        }else{
            return res.json(dbResult);
        }
    });
}

const getOwnedAccounts = (req, res) => {
    account.getByUserId(req,function(err, dbResult){
        if(err){
            return res.json(err);
        }else{
            return res.json(dbResult);
        }
    });
}

const addAccount = (req, res) => {
    if(req.userId && req.body.name){
        account.add(sanitizer.escape(req.body.name),req.userId,function(err, dbResult){
            if(err){
                return res.json(err);
            }else{
                return res.json({status:"success",message:"New account added succesfully!"});
            }
        });
    }else{
        console.log(req.body);
        return res.json({status:"error",message:"Please fill all fields."});
    }
}

const addUserToAccount = (req, res) => {
    if(emailvalidator.validate(req.body.email) && req.body.id){
        user.getByEmail(req.body.email,function(err, dbResult){
            if(dbResult.length === 0){
                return res.json({status:"error",message:"No users found with the matching email."});
            }
            let hasAccessToAccount = false;
            let uid = dbResult[0].user_ID;
            account.getOwnerById(req.userId, req.body.id, function(err, dbResult){
                if(err){
                    return res.json(err);
                }
                for(let i=0;i<dbResult.length;i++){
                    if(dbResult[i].account_ID === req.body.id && dbResult[i].owner === req.userId){
                        hasAccessToAccount = true;
                        console.log("TEST");
                    }
                }
                if(!hasAccessToAccount){
                    return res.json({status:"error",message:"You do not have access to this account."});
                }
                console.log(dbResult[0]);
                account.getExact(uid, req.body.id, function(err, dbResult){
                    if(err){
                        return res.json({status:"error",message:err});
                    }
                    if(dbResult.length > 0){
                        return res.json({status:"error",message:"User already has access to this account."});
                    }
                    account.addUser(uid, req.body.id, function(err, dbResult){
                        if(err){
                            return res.json(err);
                        }else{
                            return res.json({status:"success",message:req.body.email+" now has access to account "+req.body.id+"."});
                        }
                    });
                });
            });
        });
        
    }else{
        return res.json({status:"error",message:"Please fill all fields"});
    }
}

const updateBalance = (req, res) => {
    if(req.body.card_number && req.body.amount && req.body.action){
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

            account.getByCardNumber(req.body.card_number, function(err, dbResult){
                if(err){
                    return res.json({status:"error",message:err});
                }
                if(dbResult.length > 0){
                    let curbalance = dbResult[0].balance;
                    if(req.body.action === "0" && dbResult[0].card_type === 0){
                        if(dbResult[0].balance >= req.body.amount){
                            account.updateBalance(-Math.abs(req.body.amount), req.body.card_number, function(err, dbResult){
                                curbalance += -Math.abs(req.body.amount);
                                return res.json({status:"success",balance:curbalance});
                            });
                        }else{
                            return res.json({status:"error",message:"Not enough balance."});
                        }
                    }else if(req.body.action === "0" && dbResult[0].card_type === 1){
                        account.updateBalance(-Math.abs(req.body.amount), req.body.card_number, function(err, dbResult){
                            if(err){
                                return res.json({status:"error",message:err});
                            }
                            if(dbResult.affectedRows > 0){
                                curbalance += -Math.abs(req.body.amount);
                                return res.json({status:"success",balance:curbalance});
                            }
                        });
                    }else if(req.body.action === "1"){
                        account.updateBalance(Math.abs(req.body.amount), req.body.card_number, function(err, dbResult){
                            if(err){
                                return res.json({status:"error",message:err});
                            }
                            if(dbResult.affectedRows > 0){
                                curbalance += Math.abs(req.body.amount);
                                return res.json({status:"success",balance:curbalance});
                            }
                        });
                    }else{
                        return res.json({status:"error",message:"Invalid value for 'action'."});
                    }
                }
            });
        });
    }else{
        return res.json({status:"error",message:"Please fill all fields."});
    }
}

const deleteAccount = (req, res) => {
    if(req.body.id){
        account.getOwnerById(req.userId, req.body.id, function(err, dbResult){
            if(err){
                return res.json({status:"error",message:err});
            }
            let hasAccessToAccount = false;
            let accountId = null;
            for(let i=0;i<dbResult.length;i++){
                if(dbResult[i].account_ID === req.body.id && dbResult[i].owner === req.userId){
                    hasAccessToAccount = true;
                    accountId = dbResult[i].account_ID;
                    if(dbResult[i].balance !== 0){
                        return res.json({status:"error",message:"Account's balance must be 0 before deleting."});
                    }
                }
            }
            if(!hasAccessToAccount){
                return res.json({status:"error",message:"You are not the owner of this account."});
            }
            card.disconnectByAccountId(accountId, function(err, dbResult){
                if(err){
                    return res.json({status:"error",message:err});
                }
            });
            account.delete(accountId, function(err, dbResult){
                if(err){
                    return res.json({status:"error",message:err});
                }
                if(dbResult.affectedRows > 0){
                    return res.json({status:"success",message:"Successfully deleted account "+req.body.id});
                }
            });
        });
    }else{
        return res.json({status:"error",message:"Please fill all fields."});
    }
}

module.exports = {
    getAll,
    getById,
    getOwnedAccounts,
    addAccount,
    addUserToAccount,
    updateBalance,
    deleteAccount,
}
