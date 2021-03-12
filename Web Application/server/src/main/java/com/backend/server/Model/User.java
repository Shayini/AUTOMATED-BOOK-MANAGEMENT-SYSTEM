package com.backend.server.Model;

import javax.persistence.*;

@Entity // This tells Hibernate to make a table out of this class.
@Table(name="user") //user is the name of the table.
public class User {

    public User() {
        super();
    }

    @Id //@Id is used to generate the unique identifier for the objects of persistent class.
    @GeneratedValue(strategy= GenerationType.AUTO)
        //@GeneratedValue generates the value for the column of database table.
        // In case of GenerationType. IDENTITY , value is set by table itself that should be unique.

    //Attributes for the User Entity
    private Integer userID;
    private String userName;
    private String password;
    private String email;
    private String status;

    //Getter and Setter Methods for the attributes
    public Integer getUserID() {

        return userID;
    }

    public void setUserID(Integer userID) {

        this.userID = userID;
    }

    public String getUserName() {

        return userName;
    }

    public void setUserName(String userName) {

        this.userName = userName;
    }

    public String getPassword() {

        return password;
    }

    public void setPassword(String password) {

        this.password = password;
    }

    public String getEmail() {

        return email;
    }

    public void setEmail(String email) {

        this.email = email;
    }

    public String getStatus() {

        return status;
    }

    public void setStatus(String status) {

        this.status = status;
    }

}
