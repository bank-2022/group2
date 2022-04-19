var express = require('express');
var router = express.Router();
const db = require("../config/db");
const cardController = require("../controllers/cardController");
const jwtAuth = require("../config/jwtAuth");
const card = require('../models/cardModel');

router.get('/getAllCards', jwtAuth.verifyToken, cardController.getAll)

router.get("/info/:card_number", jwtAuth.verifyToken, cardController.getCardAccountInfoByNumber);

router.get("/info", jwtAuth.verifyToken, cardController.getCardAccountInfo);

router.get("/tries/:card_number", cardController.getTries);

router.get('/:card_number', jwtAuth.verifyToken, cardController.getByCardNumber);

router.get('/', jwtAuth.verifyToken, cardController.getByUserID);

router.put("/updateCard", jwtAuth.verifyToken, cardController.updateCardStatus);

router.put("/connect", jwtAuth.verifyToken, cardController.connectCard);

router.post("/", jwtAuth.verifyToken, cardController.addCard);

router.post("/auth", cardController.authenticate)

router.delete("/", jwtAuth.verifyToken, cardController.disconnectCard)

module.exports = router;
