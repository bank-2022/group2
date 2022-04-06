const emailvalidator = require("email-validator");
const sanitizer = require("sanitizer");
const { json } = require("express/lib/response");
const account = require("../models/accountModel");
const user = require("../models/userModel");

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
                console.log(err);
                return res.json(err);
            }else{
                if(dbResult.affectedRows > 0){
                    return res.json({status:"success",message:"New account added succesfully!"});
                }else{
                    return res.json({status:"error"});
                }
            }
        });
    }else{
        return res.json({status:"error",message:"Please fill all fields."});
    }
}

const addUserToAccount = (req, res) => {
    if(emailvalidator.validate(req.body.email) && req.body.account){
        user.getByEmail(req.body.email,function(err, dbResult){
            if(err){
                return res.json(err);
            }
            if(dbResult.length > 0){
                account.addUser(dbResult[0].user_ID,req.body.account,function(err, dbResult){
                    if(err){
                        return res.json(err);
                    }else{
                        return res.json({status:"success",message:req.body.email+" now has access to account "+req.body.account+"."});
                    }
                });
            }else{
                return res.json({status:"error",message:"Requested email not found."});
            }
        });
        
    }
}

const deleteAccount = (req, res) => {
    if(req.body.id){
        account.getOwnerById(req.userId, req.body.id, function(err, dbResult){
            if(err){
                return res.json({status:"error",message:err});
            }
            let hasAccessToAccount = false;
            for(let i=0;i<dbResult.length;i++){
                if(dbResult[i].account_ID === req.body.id && dbResult[i].owner === req.userId){
                    hasAccessToAccount = true;
                }
            }
            if(!hasAccessToAccount){
                return res.json({status:"error",message:"User does not have access to this account."});
            }
            account.delete(req.body.id, function(err, dbResult){
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
    deleteAccount,
}