var express = require('express');
var router = express.Router();
const db = require("../config/db");
const logsController = require("../controllers/logsController");
const jwtAuth = require("../config/jwtAuth");

router.get("/",jwtAuth.verifyToken,logsController.getAll);

router.get("/",jwtAuth.verifyToken,logsController.getByAccountId)

router.post("/create",jwtAuth.verifyToken,logsController.add);

router.delete("/delete",jwtAuth.verifyToken,logsController.deleteLogs);

module.exports = router;