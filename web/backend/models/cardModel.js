const db = require("../config/db");
const bcrypt = require("bcrypt");

const saltRounds = 10;
const card = {
    get: function(callback){
        return db.query("SELECT * FROM card", callback);
    },
    getByNumber: function(card_number, callback){
        return db.query("SELECT * FROM card WHERE card_number = ?", [card_number], callback);
    },
    getByUserID: function(user_ID, callback){
        return db.query("SELECT * FROM card WHERE user_ID = ?", [user_ID], callback);
    },
    getCardAccountInfo: function(userId, callback){
        return db.query("SELECT card.card_number, card.card_type, account.account_ID, account.name, account.balance FROM card JOIN account ON account.account_ID = card.account_ID WHERE card.user_ID = ?",
        [userId], callback);
    },
    getCardAccountInfoByNumber: function(card_number, callback){
        return db.query("SELECT card.card_number, card.card_type, account.account_ID, account.name, account.balance FROM card JOIN account on account.account_ID = card.account_ID WHERE card_number = ?",
        [card_number], callback);
    },
    getTries: function(card_number, callback){
        return db.query("SELECT tries FROM card WHERE card_number = ?", [card_number], callback);
    },
    updateActiveStatus: function(active, card_number, callback){
        return db.query("UPDATE card SET active = ? WHERE card_number = ?", [active, card_number], callback);
    },
    connectCard: function(accountId, userId, card_type, pin, callback){
        bcrypt.hash(pin, saltRounds, function(err, hash){
            return db.query("UPDATE card SET account_ID = ?, user_ID = ?, card_type = ?, pin = ?, active = 1 WHERE user_ID IS NULL AND account_ID IS NULL ORDER BY card_ID ASC LIMIT 1",
            [accountId, userId, card_type, hash], callback);
        });
    },
    disconnectCard: function(card_number, callback){
        return db.query("UPDATE card SET account_ID = NULL, user_ID = NULL, card_type = 0, active = 0 WHERE card_number = ?",
        [card_number], callback);
    },
    disconnectByAccountId: function(accountId, callback){
        return db.query("UPDATE card SET account_ID = NULL, user_ID = NULL, card_type = 0, active = 0 WHERE account_ID = ?",
        [accountId], callback);
    },
    add: function(callback){
        bcrypt.hash("1234", saltRounds, function(err, hash){
            return db.query("INSERT INTO card (pin, card_number, active) VALUES(?,?, 0)",
            [hash, Math.floor(Math.random() * 899999999999 + 100000000000)], callback);
        });
    },
    delete: function(req, callback){
        return db.query("DELETE FROM card WHERE card_number = ?", [req.body.card_number], callback);
    },
    updateTries: function(tries, card_number, callback){
        return db.query("UPDATE card SET tries = ? WHERE card_number = ?", [tries, card_number], callback);
    }

}

module.exports = card;
