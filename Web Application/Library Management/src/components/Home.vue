<template>
    <div class="home">
    	<div class="menu-bar">       
            <ul>
                <li><button class="login" onclick="document.getElementById('id01').style.display='block'" style="width:auto;">Login</button></li>
            </ul>
        </div>
        <div class="title"><h1>Automated Book Management System</h1></div>
        <div class="robotdiv"><img src="/static/img/robot.jpeg" alt="Robot" class="robot"></div>

        <div class="modal" id="id01">
            <form class="modal-content animate">
                <div class="imgcontainer">
                    <span onclick="document.getElementById('id01').style.display='none'" class="close" title="Close Modal">&times;</span>
                    <img src="/static/img/Welcome-Sticky-Note.jpg" alt="Avatar" class="avatar">
                </div>
                <div class="container">
                    <input class="login1" type="text" name="username" id="username" placeholder="Enter Username" v-model.trim="username" required/>

                    <input class="login1" type="password" name="password" id="password" placeholder="Enter Password" v-model.trim="password" required/>
                    
                    <input class="login1" type="button" name="submit" id="submit" value="Submit" v-on:click="submit_fun()" />
                </div>
            </form>
        </div>
    </div>
</template>

<script>
import router from '../router'
import axios from 'axios';

export default {
    name: 'Home',
    data() {
        return {
            username: '',
            password: '',
        }
    },
    methods: {
        submit_fun: async function(){
            if(this.username != "" && this.password != "") {
                await axios.get('http://localhost:8080/Project/REST-API/login?userName=' + this.username + '&password=' + this.password)
                .then(response => {
                    console.log(response.data);
                    // console.log(response.data.length);
                    // if(response.data.length >= 1) {
                    if(response.data == null) {
                        alert("Username or Password is incorrect");
                    } else if(response.data.userName === this.username && response.data.password === this.password) {
                        alert("Signin Successfull");
                        router.push("/displaybook");
                            // router.push({name: 'SignIn'});
                    } else {
                        alert("Username or Password is incorrect");
                    }
                })
                .catch(error => {
                    console.log(error);
                })
            } else {
                alert("A username and password must be present");
            }
        }
    }
}
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style>
    body {
        background-color: rgb(190, 213, 231);
        /* background-image: url("/static/img/robot2.jpg");
        width: 100%;
        height: 100%; */
    }
</style>
