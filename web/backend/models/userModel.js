const db = require("../config/db");
const bcrypt = require("bcrypt");

const saltRounds = 10;
const user = {
    get: function(callback){
        return db.query("SELECT * FROM user", callback);
    },
    getById: function(id, callback){
        return db.query("SELECT * FROM user WHERE user_ID = ?", [id], callback);
    },
    getByEmail: function(email, callback){
        return db.query("SELECT * FROM user WHERE email = ?", [email], callback);
    },
    getCredentials: function(email, password, callback){
        return db.query("SELECT email, password FROM user WHERE email = ? AND password = ?",
        [email, password], callback);
    },
    add: function(req, callback){
        bcrypt.hash(req.body.password, saltRounds, function(err, hash){
            return db.query("INSERT INTO user (email, password, fname, lname, address, phone) VALUES(?,?,?,?,?,?)",
            [req.body.email, hash, req.body.fname, req.body.lname, req.body.address, req.body.phone], callback);
        });
    },
    updateAvatar: function(userID, file, callback){
        return db.query("UPDATE user SET picture = ? WHERE user_ID = ?",
        [file, userID], callback);
    }

}

module.exports = user;
