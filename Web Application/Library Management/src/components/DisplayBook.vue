<template>
    <div class="displaybooks">
    	<div class="menu-bar">
            <ul>
                <li><router-link class="link" to="/">Logout</router-link></li>
                <li><button class="add" onclick="document.getElementById('id01').style.display='block'" style="width:auto;">Add</button></li>
            </ul>
    	</div>

        <div>
            <h1>Book Details</h1>
        </div>
        <div>
            
            <input type="text" list="my-list-id" v-model="searchtitle" placeholder="Filter By"/>
            <datalist id="my-list-id">
                <option v-for="list in lists" v-bind:key="list">{{ list }}</option>
            </datalist>

            <input type="text" v-model="search" placeholder="Search"/>
        </div>
        <div class="displaybook_box">
            <table class="redTable" v-if="bookDetail && bookDetail.length">
                <thead>
                    <tr>
                        <td><h3>Book ID</h3></td>
                        <td><h3>Name</h3></td>
                        <td><h3>Author</h3></td>
                        <td><h3>Shelf</h3></td>
                        <td><h3>CopyNo</h3></td>
                        <td><h3>Status</h3></td>
                    </tr>
                </thead>
                <tr v-for="(element, i) in filteredBookDetail" :key="element.id">
                    <td>{{element.bookID}}</td>
                    <td>{{element.bookName}}</td>
                    <td>{{element.author}}</td>
                    <td>{{element.shelf}}</td>
                    <td>{{element.copyNo}}</td>
                    <td>{{element.status}}</td>
                    <!-- <td><button class="delete" type="submit" value="Delete" @click="deleteBook(element.bookID, i)">Delete</button></td> -->
                    <td><button class="edit" onclick="document.getElementById('id02').style.display='block'" @click="getBook(element.bookID, i)" style="width:auto;">Edit</button></td>
                </tr>
            </table>
        </div>

        <div class="modal1" id="id01">
            <form class="modal1-content animate">
                <div class="imgcontainer">
                    <span onclick="document.getElementById('id01').style.display='none'" class="close" title="Close Modal">&times;</span>
                    <img src="/static/img/1.jpg" alt="Avatar" class="avatar">
                </div>
                <div class="container">
                    <label><b>Add Book Details</b></label>
                    <input class="addbook" type="text" name="bookname" id="bookname" placeholder="Book Name" v-model.trim="bookname" required/>

                    <input class="addbook" type="text" name="author" id="author" placeholder="Author" v-model.trim="author" required/>
            
                    <input class="addbook" type="text" name="shelf" id="shelf" placeholder="Shelf" v-model.trim="shelf" required/>

                    <input class="addbook" type="text" name="copynumber" id="copynumber" placeholder="Copy Number" v-model.trim="copynumber" required/>
                    
                    <input class="addbook" type="button" name="submit" id="submit" value="Submit" v-on:click="addBook()" />
                </div>
            </form>
        </div>

        <div class="modal1" id="id02">
            <form class="modal1-content animate">
                <div class="imgcontainer">
                    <span onclick="document.getElementById('id02').style.display='none'" class="close" title="Close Modal">&times;</span>
                    <img src="/static/img/1.jpg" alt="Avatar" class="avatar">
                </div>
                <div class="container">
                    <label><b>Edit Book Details</b></label>
                    <input class="addbook" type="text" name="bookname1" id="bookname1" placeholder="Book Name" v-model.trim="bookname" required/>

                    <input class="addbook" type="text" name="author1" id="author1" placeholder="Author" v-model.trim="author" required/>
            
                    <input class="addbook" type="text" name="shelf1" id="shelf1" placeholder="Shelf" v-model.trim="shelf" required/>

                    <input class="addbook" type="text" name="copynumber1" id="copynumber1" placeholder="Copy Number" v-model.trim="copynumber" required/>
                    
                    <input class="addbook" type="text" name="status1" id="status1" placeholder="Status" v-model.trim="status" required/>

                    <input class="addbook" type="button" name="submit" id="submit" value="Update" v-on:click="updateBook()" />
                    <button class="addbook" type="button" name="delete" id="delete" value="Delete" @click="deleteBookX()">Delete</button>
                </div>
            </form>
        </div>
    </div>
</template>

<script>
import router from '../router'
import axios from 'axios';

export default {
    name: 'DisplayBook',
    data () {
        return {
            bookDetail: [],
            book: [],
            bookname: '',
            author: '',
            shelf: '',
            copynumber: '',
            status: '',
            id: '',
            i: '',
            resp: '',
            search: '',
            searchtitle: '',
            searchtitlenew: '',
            lists: ["Book ID", "Book Name", "Author", "Shelf", "Status", "Show All"]
        }
    },
    created() {
        //this.username = this.$route.params.id;
        this.getBookDetail();
    },
    computed: {
        filteredBookDetail: function() {
            this.searchtitlenew = (this.searchtitle == "Book ID") ? "bookID" : ((this.searchtitle == "Book Name") ? "bookName" : ((this.searchtitle == "Author") ? "author" : ((this.searchtitle == "Shelf") ? "shelf" : ((this.searchtitle == "Status") ? "status" : this.searchtitle))));
            if(this.searchtitlenew=="bookName" || this.searchtitlenew=="author" || this.searchtitlenew=="shelf" || this.searchtitlenew=="status") {
                return this.bookDetail.filter((row) => {
                    return row[this.searchtitlenew].toLowerCase().match(this.search.toLowerCase())
                });
            } else if(this.search=='') {
                return this.bookDetail.filter((row) => {
                    console.log(row);
                    return row
                });
            } else if(this.searchtitlenew=="bookID") {
                return this.bookDetail.filter((row) => {
                    console.log(row);
                    return row[this.searchtitlenew]==this.search
                });
            } else {
                return this.bookDetail.filter((row) => {
                    return row
                });
            }
        }
    },
    methods: {
        getBookDetail: async function() {
            await axios.get('http://localhost:8080/Project/REST-API/bookdetails')
            .then(response => {
                this.bookDetail = response.data;
                console.log(this.bookDetail);
            })
            .catch(error => {
                console.log(error);
            })
        },
        deleteBook: async function(bookid, i) {
            await axios.delete('http://localhost:8080/Project/REST-API/deletebook/' + bookid)
            .then(response => {
                console.log(response);
                console.log(i);
                this.bookDetail.splice(i, 1);
            })
        },
        getBook: async function(bookid, i) {
            this.book = this.bookDetail[i];
            console.log(this.book);
            console.log(this.book.bookName);
            document.getElementById('bookname1').value = this.book.bookName;
            document.getElementById('author1').value = this.book.author;
            document.getElementById('shelf1').value = this.book.shelf;
            document.getElementById('copynumber1').value = this.book.copyNo;
            document.getElementById('status1').value = this.book.status;
            this.bookname = this.book.bookName;
            this.author = this.book.author;
            this.shelf = this.book.shelf;
            this.copynumber = this.book.copyNo;
            this.status = this.book.status;
            this.id = bookid;
            this.i = i;
        },
        updateBook: async function() {
            if(this.bookname && this.author && this.shelf && this.copynumber && this.status){

                await axios.put('http://localhost:8080/Project/REST-API/updatebook2/' + this.id, {
                    bookName: this.bookname,
                    author: this.author,
                    shelf: this.shelf,
                    copyNo: this.copynumber,
                    status: this.status
                })
                .then(response => {
                    console.log(response);
                })
                .catch(function (error) {
                    currentObject.resp = error;
                });

                alert("Successfully updated");
                document.getElementById('id02').style.display='none';

                this.getBookDetail();
            }
        },
        deleteBookX: async function() {
            await axios.delete('http://localhost:8080/Project/REST-API/deletebook/' + this.id)
            .then(response => {
                console.log(response);
                console.log(this.i)
                this.bookDetail.splice(this.i, 1);
            })

            document.getElementById('id02').style.display='none';
        },
        addBook: async function(){
            if(this.bookname && this.author && this.shelf && this.copynumber){

                let currentObject = this;
                await axios.post('http://localhost:8080/Project/REST-API/addbook', {
                    bookName: this.bookname,
                    author: this.author,
                    shelf: this.shelf,
                    copyNo: this.copynumber,
                    status: 'Available'
                })
                .then(function (response) {
                    currentObject.resp = response.data;
                    console.log(response);
                })
                .catch(function (error) {
                    currentObject.resp = error;
                });

                alert("Successfully added");
                document.getElementById('id01').style.display='none';

                this.getBookDetail();
                //window.location.reload();
            }else{
                alert("Unsuccessfull\nPlease fill the required fields");
            }
        }
    }
}
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style>
    
</style>
