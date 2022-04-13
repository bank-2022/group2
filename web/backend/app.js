var createError = require('http-errors');
var express = require('express');
var path = require('path');
var cookieParser = require('cookie-parser');
var logger = require('morgan');
var history = require('connect-history-api-fallback');
var cors = require("cors");
var bcrypt = require("bcrypt");
var sanitizer = require("sanitizer");
var db = require("./config/db");
require("dotenv").config();

const vue = express.static("../frontend/dist");
var userRouter = require('./routes/userRoutes');
let cardRouter = require('./routes/cardRoutes');
let accountRouter = require("./routes/accountRoutes");
let logsCrouter = require('./routes/logsRoutes');

var app = express();

// view engine setup
app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'jade');
app.set('etag',false)


app.use(logger('dev'));
app.use(express.json());
app.use(express.urlencoded({ extended: false }));
app.use(cookieParser());
app.use(express.static(path.join(__dirname, 'public')));
app.use(cors({origin: "*"}));
//app.use(vue);
app.use('/uploads', express.static('uploads'));

app.use((err, req, res, next) => {
  console.error(err.stack)
  res.status(500).send('err 500')
})

app.use('/api/user', userRouter);
app.use('/api/card', cardRouter);
app.use("/api/account", accountRouter);
app.use("/api/logs", logsCrouter);

app.use(history({
  index: "index.html"
}));
app.use(vue);
// catch 404 and forward to error handler
app.use(function(req, res, next) {
  next(createError(404));
});

// error handler
app.use(function(err, req, res, next) {
  // set locals, only providing error in development
  res.locals.message = err.message;
  res.locals.error = req.app.get('env') === 'development' ? err : {};

  // render the error page
  res.status(err.status || 500);
  res.render('error');
});

module.exports = app;
