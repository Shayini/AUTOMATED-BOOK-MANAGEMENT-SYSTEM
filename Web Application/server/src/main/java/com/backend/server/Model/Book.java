package com.backend.server.Model;

import javax.persistence.*;

@Entity // This tells Hibernate to make a table out of this class.
@Table(name="book") //user is the name of the table.

public class Book {

    public Book() {
        super();
    }

    @Id //@Id is used to generate the unique identifier for the objects of persistent class.
    @GeneratedValue(strategy= GenerationType.AUTO)
    //@GeneratedValue generates the value for the column of database table.
    // In case of GenerationType. IDENTITY , value is set by table itself that should be unique.

    //Attributes for the User Entity
    //private String bookID;
    private Integer bookID;
    //private Integer bookNo;
    private String bookName;
    private String author;
    private String shelf;
    private Integer copyNo;
    private String status;

    //Getter and Setter Methods for the attributes
    public Integer getBookID() {

        return bookID;
    }

    public void setBookID(Integer bookID) {

        this.bookID = bookID;
    }

//    public Integer getBookNo() {
//
//        return bookNo;
//    }
//
//    public void setBookNo(Integer bookNo) {
//
//        this.bookNo = bookNo;
//    }

    public String getBookName() {

        return bookName;
    }

    public void setBookName(String bookName) {

        this.bookName = bookName;
    }

    public String getAuthor() {
        return author;
    }

    public void setAuthor(String author) {
        this.author = author;
    }

    public String getShelf() {

        return shelf;
    }

    public void setShelf(String shelf) {

        this.shelf = shelf;
    }

    public Integer getCopyNo() {

        return copyNo;
    }

    public void setCopyNo(Integer copyNo) {

        this.copyNo = copyNo;
    }

    public String getStatus() {

        return status;
    }

    public void setStatus(String status) {

        this.status = status;
    }

}
