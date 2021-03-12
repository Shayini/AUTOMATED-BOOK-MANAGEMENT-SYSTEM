package com.backend.server.Repository;

import com.backend.server.Model.User;
import org.springframework.data.jpa.repository.Modifying;
import org.springframework.data.repository.CrudRepository;
import org.springframework.transaction.annotation.Transactional;

import java.util.Optional;

/*
  In a typical Java application, you’d expect to write a class that implements ProjectRepository.
  But that’s what makes Spring Data JPA so powerful: You don’t have to write an implementation of the repository interface.
*/

// This will be AUTO IMPLEMENTED by Spring into a Bean called ProjectRepository.
// CRUD refers Create, Read, Update, Delete.

public interface User_repository extends CrudRepository<User,String>

        /*By extending CrudRepository, ProjectRepository inherits several methods for working with Project persistence,
         including methods for saving, deleting, and finding Project entities.*/

        //Spring Data JPA also allows you to define other query methods.
        // In this case of ProjectRepository, this is shown with findByProjectId(),findByProjectAuthor() and findByProjectName() methods.

{

    Optional<User> findByUserID(Integer userID);
    Optional<User> findByUserName(String userName);
    Optional<User> findByEmail(String email);
    Optional<User> findByStatus(String status);
    Iterable<User> findAll();
    @Modifying
    @Transactional
    void deleteByUserName(String UserName);
}
