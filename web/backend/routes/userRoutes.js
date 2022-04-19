var express = require('express');
var router = express.Router();
const db = require("../config/db");
const userController = require("../controllers/userController");
const jwtAuth = require("../config/jwtAuth");

router.get("/info",jwtAuth.verifyToken,userController.userInfo);

router.get('/',jwtAuth.verifyToken,userController.getAll)

router.get('/:id',jwtAuth.verifyToken,userController.getById);

router.post("/login",userController.userLogin);

router.post("/register",userController.userRegister);

router.put("/avatar", jwtAuth.verifyToken, userController.updateAvatar);

module.exports = router;