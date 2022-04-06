const db = require("../config/db");
const bcrypt = require("bcrypt");

const account = {
    get: function(callback){
        return db.query("SELECT * FROM account", callback);
    },
    getById: function(id, callback){
        return db.query("SELECT * FROM account WHERE account_ID = ?", [id], callback);
    },
    getByUserId: function(req, callback){
        return db.query("SELECT account.* FROM account JOIN user_account ON user_account.account_ID = account.account_ID JOIN user ON user.user_ID = user_account.user_ID WHERE user.user_ID = ?",
        [req.userId], callback);

    },
    getByCardId: function(id, callback){
        return db.query("SELECT * FROM account JOIN card on card.account_ID = account.account_ID WHERE account_ID = ?",
        [id], callback);

    },
    getOwnerById: function(userId, accountId, callback){
        return db.query("SELECT account.account_ID, account.owner FROM account JOIN user_account ON user_account.account_ID = account.account_ID JOIN user ON user.user_ID = user_account.user_ID WHERE user.user_ID = ? AND account.account_ID = ?",
        [userId, accountId], callback);
    },
    add: function(name, userId, callback){
        db.query("INSERT INTO account(name, date_opened, owner) VALUES(?, current_timestamp(), ?)", [name, userId]);
        return db.query("INSERT INTO user_account(user_ID, account_ID) VALUES(?, (SELECT account_ID FROM account ORDER BY account_ID DESC LIMIT 1))",[userId], callback);
    },
    addUser: function(userId, accountId, callback){
        return db.query("INSERT INTO user_account(user_ID, account_ID) VALUES(?, ?)", [userId, accountId], callback);
    },
    delete: function(accountId, callback){
        return db.query("DELETE FROM account WHERE account_ID = ?", [accountId], callback);
    }
}

module.exports = account;