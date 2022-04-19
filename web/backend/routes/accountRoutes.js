var express = require('express');
var router = express.Router();
const db = require("../config/db");
const accountController = require("../controllers/accountController");
const jwtAuth = require("../config/jwtAuth");
const account = require('../models/accountModel');

router.get("/",jwtAuth.verifyToken,accountController.getOwnedAccounts);

router.get("/:id",jwtAuth.verifyToken,accountController.getById);

router.post("/adduser",jwtAuth.verifyToken,accountController.addUserToAccount);

router.post("/",jwtAuth.verifyToken,accountController.addAccount);

router.put("/update", jwtAuth.verifyToken, accountController.updateBalance);

router.delete("/", jwtAuth.verifyToken, accountController.deleteAccount);

module.exports = router;
